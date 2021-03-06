#include "SceneHierarchyPanel.h"

#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Components.h"

SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene> scene) {
	SetContext(scene);
}

void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene> scene) {
	context = scene;
	selectionContext = entt::null;
}

void SceneHierarchyPanel::OnImguiRender() {
	ImGui::Begin("Scene Hierarchy");
	context->Registry.each([&](entt::entity entity) {
		DrawEntityNode(entity);
	});

	if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
		selectionContext = entt::null;
	}

	// Right-click on a blank space
	if (ImGui::BeginPopupContextWindow("testing", ImGuiMouseButton_Right, false)) {
		if (ImGui::MenuItem("Create Empty Entity")) {
			context->CreateEntity("Unnamed Entity");
		}
		ImGui::EndPopup();
	}
	ImGui::End();

	ImGui::Begin("Properties");
	if (selectionContext != entt::null) {
		DrawComponents(selectionContext);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");
		
		if (ImGui::BeginPopup("AddComponent")) {
			if (ImGui::MenuItem("Camera")) {
				context->Reg().emplace<CameraComponent>(selectionContext);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Light")) {
				context->Reg().emplace<LightComponent>(selectionContext);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Line")) {
				context->Reg().emplace<LineComponent>(selectionContext);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Line Generator")) {
				context->Reg().emplace<LineGeneratorComponent>(selectionContext);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Line Renderer")) {
				if(context->Reg().all_of<LineComponent>(selectionContext) || context->Reg().all_of<LineGeneratorComponent>(selectionContext))
					context->Reg().emplace<LineRendererComponent>(selectionContext);
				else
					std::cout << "Entity needs to have a LineComponent or LineGenerator before adding a LineRendererComponent" << std::endl;
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Mesh (Manual)")) {
				context->Reg().emplace<MeshComponent>(selectionContext);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Mesh (Load OBJ)")) {
				context->Reg().emplace<MeshObjLoaderComponent>(selectionContext);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Mesh Renderer")) {
				if (context->Reg().any_of<MeshComponent, MeshObjLoaderComponent>(selectionContext))
					context->Reg().emplace<MeshRendererComponent>(selectionContext);
				else
					std::cout << "Entity needs to have a MeshComponent before adding a MeshRendererComponent" << std::endl;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void SceneHierarchyPanel::DrawEntityNode(entt::entity entity) {
	auto& tc = context->Registry.get<TagComponent>(entity);

	// don't open tree node when click on name, but only on arrow only
	ImGuiTreeNodeFlags flags = ((selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0)
		| ImGuiTreeNodeFlags_OpenOnArrow;
	const char* label = tc.Tag.c_str();
	bool opened = ImGui::TreeNodeEx(label, flags);
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
		selectionContext = entity;
	}

	bool isEntityDeleted = false;
	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Delete Entity")) {
			isEntityDeleted = true;
			if (selectionContext == entity) {
				selectionContext = entt::null;
			}
		}
		ImGui::EndPopup();
	}

	if (opened) {
		ImGui::TreePop();
	}

	if (isEntityDeleted) {
		context->DestroyEntity(entity);
	}
}

static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
	bool fancy = true;
	if (!fancy) {
		ImGui::PushID(label.c_str());
		ImGui::Text(label.c_str()); ImGui::SameLine();
		float& x = values[0];
		ImGui::DragFloat("X", &(values.x), 0.1f, 0.0f, 0.0f, "%.3f"); ImGui::SameLine();
		float& y = values[1];
		ImGui::DragFloat("Y", &y); ImGui::SameLine();
		ImGui::DragFloat("Z", &values.z);
		ImGui::PopID();
		return;
	
	}

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	// https://coolors.co/ff595e-ffca3a-8ac926-1982c4-6a4c93
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 1.0f, 0.34901961f, 0.36862745f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 1.000000f, 0.521569f, 0.537255f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 1.000000f, 0.200000f, 0.227451f, 1.0f });

	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.541176f, 0.788235f, 0.149020f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.631373f, 0.858824f, 0.262745f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.462745f, 0.670588f, 0.129412f, 1.0f });
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.098039f, 0.509804f, 0.768627f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.149020f, 0.607843f, 0.890196f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.082353f, 0.423529f, 0.635294f, 1.0f });
	if (ImGui::Button("Z", buttonSize))
		values.z = resetValue;
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

bool SceneHierarchyPanel::AddComponentSettingsButton() {
	ImGui::SameLine(ImGui::GetWindowWidth() - 35.0f);
	if (ImGui::Button("...", ImVec2{ 30, 20 }))
	{
		ImGui::OpenPopup("ComponentSettings");
	}
	//ImGui::PopStyleVar();

	bool removeComponent = false;
	if (ImGui::BeginPopup("ComponentSettings"))
	{
		if (ImGui::MenuItem("Remove component"))
			removeComponent = true;

		ImGui::EndPopup();
	}
	return removeComponent;
}

const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

void DrawComponentParametersUI(CameraComponent& cameraComponent) {
	auto& camera = cameraComponent.Camera;

	ImGui::Checkbox("Primary", &cameraComponent.Primary);

	const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
	const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
	if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
	{
		for (int i = 0; i < 2; i++)
		{
			bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
			if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
			{
				currentProjectionTypeString = projectionTypeStrings[i];
				camera.SetProjectionType((SceneCamera::ProjectionType)i);
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
		float verticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
		if (ImGui::DragFloat("Vertical FOV", &verticalFOV)) {
			camera.SetPerspectiveVerticalFOV(glm::radians(verticalFOV));
		}

		float near = camera.GetPerspectiveNearClip();
		if (ImGui::DragFloat("Near", &near)) {
			camera.SetPerspectiveNearClip(near);
		}

		float far = camera.GetPerspectiveFarClip();
		if (ImGui::DragFloat("Far", &far)) {
			camera.SetPerspectiveFarClip(far);
		}
	}
	else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {
		float size = camera.GetOrthographicSize();
		if (ImGui::DragFloat("Size", &size)) {
			camera.SetOrthographicSize(size);
		}

		float near = camera.GetOrthographicNearClip();
		if (ImGui::DragFloat("Near", &near)) {
			camera.SetOrthographicNearClip(near);
		}

		float far = camera.GetOrthographicFarClip();
		if (ImGui::DragFloat("Far", &far)) {
			camera.SetOrthographicFarClip(far);
		}

		ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
	}
}

void DrawComponentParametersUI(LightComponent& lc) {
	ImGui::DragFloat("Intensity", &lc.intensity);
}

void DrawComponentParametersUI(LineComponent& lc) {
	std::vector<glm::vec3>& vertices = lc.Vertices;
	int numVertices = (int)vertices.size();
	int ix = 0;

	for (glm::vec3& v : vertices) {
		if (ImGui::InputFloat3(std::to_string(ix).c_str(), glm::value_ptr(v), "%.3f", treeNodeFlags)) {
			lc.ComputeVertexArray();
		}
		ix++;
	}
	if (ImGui::InputInt("Count", &numVertices, 1, 10, treeNodeFlags)) {
		if (numVertices >= 2 && numVertices < 100) {
			vertices.resize(numVertices);
			lc.ComputeVertexArray();
		}
	}
}

void DrawComponentParametersUI(LineGeneratorComponent& lgc) {
	const char* shapeTypeStrings[] = { "Rectangle", "Ellipse", "Ngon", "Connector" };
	const char* currentTypeString = shapeTypeStrings[(int)lgc.type];
	if (ImGui::BeginCombo("Shape Type", currentTypeString)) {
		for (int i = 0; i < 4; i++) {
			bool isSelected = currentTypeString == shapeTypeStrings[i];
			if (ImGui::Selectable(shapeTypeStrings[i], isSelected)) {
				currentTypeString = shapeTypeStrings[i];
				lgc.type = (LineGeneratorComponent::Type)i;
				lgc.CalculateVertices();
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();

		}
		ImGui::EndCombo();
	}

	bool hasChanged = false;
	switch (lgc.type) {
	case LineGeneratorComponent::Type::Rectangle:
		hasChanged |= ImGui::DragFloat("Width", &lgc.rectangle.width);
		hasChanged |= ImGui::DragFloat("Height", &lgc.rectangle.height);
		break;
	case LineGeneratorComponent::Type::Ellipse:
		hasChanged |= ImGui::DragFloat("Radius 1", &lgc.ellipse.r1);
		hasChanged |= ImGui::DragFloat("Radius 2", &lgc.ellipse.r2);
		hasChanged |= ImGui::InputInt("# Samples", &lgc.ellipse.numSamples);
		break;
	case LineGeneratorComponent::Type::Ngon:
		hasChanged |= ImGui::InputInt("# Sides", &lgc.ngon.numSides);
		hasChanged |= ImGui::DragFloat("Radius", &lgc.ngon.radius);
		break;
	case LineGeneratorComponent::Type::Connector:
		hasChanged |= ImGui::DragFloat3("Point 1", glm::value_ptr(lgc.connector.p1));
		hasChanged |= ImGui::DragFloat3("Point 2", glm::value_ptr(lgc.connector.p2));
		hasChanged |= ImGui::DragFloat("Steepness", &lgc.connector.steepness, 1.0f, 5.0f, 50.f);
		hasChanged |= ImGui::InputInt("# Samples", &lgc.connector.numSamples);
		break;
	}
	if (hasChanged) {
		lgc.CalculateVertices();
	}
}

void DrawComponentParametersUI(LineRendererComponent& lrc) {
	glm::vec4& color = lrc.Color;
	ImGui::ColorEdit4("Color", glm::value_ptr(color));
	ImGui::Checkbox("Looped", &lrc.IsLooped);
}

void DrawComponentParametersUI(MeshComponent& mc) {
	std::vector<MeshComponent::MeshVertex>& vertices = mc.Vertices;
	int numVertices = (int)vertices.size();
	std::vector<glm::uvec3>& indices = mc.Indices;
	int numIndices = (int)indices.size();
	int i = 0;
	for (MeshComponent::MeshVertex& v : vertices) {
		if (ImGui::InputFloat3(("v" + std::to_string(i)).c_str(), glm::value_ptr(v.Position), "%.3f", treeNodeFlags)) {
			mc.ComputeVertexArray();
		}
		i++;
	}
	if (ImGui::InputInt("# Vertices", &numVertices, 1, 10, treeNodeFlags)) {
		if (numVertices >= 3 && numVertices < 100) {
			vertices.resize(numVertices);
			mc.ComputeVertexArray();
		}
	}
	ImGui::Separator();
	for (int k = 0; k < indices.size(); k++) {
		glm::uvec3& triplet = indices[k];
		if (ImGui::InputScalarN(("i" + std::to_string(k)).c_str(), ImGuiDataType_U32, glm::value_ptr(triplet), 3, (void*)1, (void*)10, "%d", treeNodeFlags)) {
			mc.ComputeVertexArray();
		}
	}
	if (ImGui::InputInt("# Triangles", &numIndices, 1, 10, treeNodeFlags)) {
		if (numIndices >= 1 && numIndices < 100) {
			indices.resize(numIndices);
			mc.ComputeVertexArray();
		}
	}
}

void DrawComponentParametersUI(MeshObjLoaderComponent& molc) {

	char buffer[256] = { 0 };
	strcpy_s(buffer, sizeof(buffer), molc.GetFilePath().c_str());

	if (ImGui::InputText("OBJ File Path", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
		molc.SetFilePath(std::string(buffer));
	}
}

void DrawComponentParametersUI(MeshRendererComponent& mrc) {
	glm::vec4& color = mrc.Color;
	ImGui::ColorEdit4("Color", glm::value_ptr(color));
	ImGui::Checkbox("IsTransparent", &mrc.IsTransparent);
}

template <typename TComp>
void SceneHierarchyPanel::DrawComponentUITreeNodeIfExists(entt::basic_handle<entt::entity> handle, void DrawCompParams(TComp&)) {
	if (!handle.all_of<TComp>()) return;

	bool isOpen = ImGui::TreeNodeEx(TComp::GetName(), treeNodeFlags);
	bool shouldRemove = SceneHierarchyPanel::AddComponentSettingsButton();
	auto& comp = handle.get<TComp>();

	if (isOpen) {
		DrawComponentParametersUI(comp);

		ImGui::TreePop();
	}

	if (shouldRemove)
		handle.remove<TComp>();
}

void SceneHierarchyPanel::DrawComponents(entt::entity entity) {
	entt::basic_handle handle{ context->Registry, entity };

	if (handle.all_of<TagComponent>()) {
		std::string& tag = handle.get<TagComponent>().Tag;
		
		char buffer[256] = { 0 };
		strcpy_s(buffer, sizeof(buffer), tag.c_str());
		if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
			tag = std::string(buffer);
		}
	}

	if (handle.all_of<TransformComponent>()) {
		if (ImGui::TreeNodeEx("Transform", treeNodeFlags)) {
			auto& transform = handle.get<TransformComponent>();
			DrawVec3Control("Translation", transform.Translation);
			glm::vec3 rotation = glm::degrees(transform.Rotation);
			DrawVec3Control("Rotation", rotation);
			transform.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", transform.Scale, 1.0f);
			ImGui::TreePop();
		}
	}

	DrawComponentUITreeNodeIfExists<CameraComponent>(handle, DrawComponentParametersUI);
	DrawComponentUITreeNodeIfExists<LightComponent>(handle, DrawComponentParametersUI);
	DrawComponentUITreeNodeIfExists<LineComponent>(handle, DrawComponentParametersUI);
	DrawComponentUITreeNodeIfExists<LineGeneratorComponent>(handle, DrawComponentParametersUI);
	DrawComponentUITreeNodeIfExists<LineRendererComponent>(handle, DrawComponentParametersUI);
	DrawComponentUITreeNodeIfExists<MeshComponent>(handle, DrawComponentParametersUI);
	DrawComponentUITreeNodeIfExists<MeshObjLoaderComponent>(handle, DrawComponentParametersUI);
	DrawComponentUITreeNodeIfExists<MeshRendererComponent>(handle, DrawComponentParametersUI);
}
