/*
 *  Robot Audio Plugins
 *  
 *  Copyright (C) 2023 Martin Bångens
 *
 *  Programing style originally from https://github.com/DISTRHO/DPF-Plugins
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "RobotHexedFilterUI.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

RobotHexedFilterUI::RobotHexedFilterUI()
    : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT),
      fResizeHandle(this),
      context(ImPlot::CreateContext())
{
    setGeometryConstraints(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true);
    dataPsdOut.resize(2400,0);

    
    glGenTextures(1, &bgImageTexture);
    glBindTexture(GL_TEXTURE_2D, bgImageTexture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // hide handle if UI is resizable
    if (isResizable())
        fResizeHandle.hide();
    
}
RobotHexedFilterUI::~RobotHexedFilterUI()
{
  ImPlot::DestroyContext(context);
}
// -----------------------------------------------------------------------
// DSP Callbacks

void RobotHexedFilterUI::parameterChanged(uint32_t index, float value)
{
    switch (index)
    {
    case 0:
        fCutOff = value;
        repaint();
        break;
    case 1:
        fRes = value;
        repaint();
        break;
    case 2:
        fMode = value;
        repaint();
        break;
    case 3:
        fWet = value;
        repaint();
        break;
    }
}

void RobotHexedFilterUI::programLoaded(uint32_t index)
{
    if (index != 0)
        return;

    // Default values
    fCutOff = 100.0f;
    fRes    = 0.0f;
    fMode   = 4.0f;
    fWet    = 0.0f;
    repaint();
}

// -----------------------------------------------------------------------
// Widget Callbacks

/**
    ImGui specific onDisplay function.
*/
void RobotHexedFilterUI::onImGuiDisplay()
{
    // Window
    const float width = getWidth();
    const float height = getHeight();
    //const float scaler = (width/600+height/400)/2;

    // -----------------------------------------------------------------------
    // Background
 
    ImGui::SetNextWindowPos(ImVec2(-8, -8));
    ImGui::SetNextWindowSize(ImVec2(width+16, height+16));
    ImGui::Begin("Background", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

    float bitmapWidth = width*0.977;
    if((lastWidth != width) && (lastHeight != height))
    {
        static std::unique_ptr<lunasvg::Document> background = lunasvg::Document::loadFromData(bg);
        lunasvg::Bitmap btbitmap = background->renderToBitmap(bitmapWidth, height); 
        btbitmap.convertToRGBA();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmapWidth, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, btbitmap.data());
    }

    ImGui::Image((void*)(intptr_t)bgImageTexture, ImVec2(bitmapWidth, height));
    ImGui::End();
    // -----------------------------------------------------------------------
    // Tabs
    float firstStop = width*0.0965-9.945;
    float tabHight =  height*0.063+(400/height)*9;
    
    ImGui::SetNextWindowPos(ImVec2(-8, -8));
    ImGui::SetNextWindowSize(ImVec2(width+16, tabHight));
    ImGui::Begin("Tabs", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(width/600);
    

   /* --->       Then fist stop 
    * ||||XXXXXXXXXXXXXXXXXXXXXXXXX||||
    * ||||XXXXXXXXXXXXXXXXXXXXXXXXX||||
    * ||||XXXXXXXXXXXXXXXXXXXXXXXXX||||
    * ||||XXXXXXXXXXXXXXXXXXXXXXXXX||||
    * ||||XXXXXXXXXXXXXXXXXXXXXXXXX||||
    */
    ImGui::Dummy(ImVec2(firstStop, 3));
    ImGui::SameLine();
    
    float b = 1.0f; //  test whatever color you need from imgui_demo.cpp e.g.
    float c = 0.5f; // 
    int i = 3;

    ImVec2 presetsV = ImVec2(width/6, tabHight);
    ImVec4 blue = (ImVec4)ImColor::HSV(0.6f, 1.3f, 1.0f);
    ImVec4 blueActive = (ImVec4)ImColor::HSV(0.6f, 1.3f, 0.8f);

if (tabPresets)
{
    ImGui::PushID("Presets");
    ImGui::PushStyleColor(ImGuiCol_Button, blueActive);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, blueActive);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, blueActive);
    ImGui::Button("Presets", presetsV);
    ImGui::PopStyleColor(3);
    ImGui::PopID();
}
else
{
    ImGui::PushID("Presets");
    ImGui::PushStyleColor(ImGuiCol_Button, blue);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, blue);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, blue);
    if(ImGui::Button("Presets",  presetsV))
    {
      tabPresets = 1;
      tabEditor = 0;
      tabAbout = 0;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopID();
}
    ImGui::SameLine();
    //ImGui::Dummy(ImVec2(width*0.055-20, scaler*36-28));
    ImGui::Dummy(ImVec2(width*0.0225-14, tabHight));
    ImGui::SameLine();
    
    ImVec2 editorV = ImVec2(width/2.335, tabHight);
if (tabEditor)
{

    ImGui::PushID("Hexed");
    ImGui::PushStyleColor(ImGuiCol_Button, blueActive);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, blueActive);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, blueActive);
    ImGui::Button("Hexed", editorV);
    ImGui::PopStyleColor(3);
    ImGui::PopID();
}
else
{
    ImGui::PushID("Hexed");
    ImGui::PushStyleColor(ImGuiCol_Button, blue);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, blue);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, blue);
    if(ImGui::Button("Hexed", editorV))
    {
      tabPresets = 0;
      tabEditor = 1;
      tabAbout = 0;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopID();
}
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(width*0.0225-14, tabHight));
    ImGui::SameLine();

    ImVec2 aboutV = ImVec2(width/6, tabHight);
if (tabAbout)
{
    ImGui::PushID("About");
    ImGui::PushStyleColor(ImGuiCol_Button, blueActive);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, blueActive);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, blueActive);
    ImGui::Button("About", aboutV);
    ImGui::PopStyleColor(3);
    ImGui::PopID();
}
else
{
    ImGui::PushID("About");
    ImGui::PushStyleColor(ImGuiCol_Button, blue);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, blue);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, blue);
    if(ImGui::Button("About", aboutV))
    {
      tabPresets = 0;
      tabEditor = 0;
      tabAbout = 1;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopID();
}


    
    ImGui::End();
 

    // -----------------------------------------------------------------------
    // Main Window 

    ImGui::SetNextWindowPos(ImVec2(((width/600)*45), tabHight));
    ImGui::SetNextWindowSize(ImVec2(width, height-tabHight));
    ImGui::Begin("Hexed Filter", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(width/600);
    if(tabEditor)
    {
    // -----------------------------------------------------------------------
    // Plot
    
    //ImGui::Dummy(ImVec2(width, (height/600)*75-75));
    //ImGui::Dummy(ImVec2(width, height*0.0995+(600/height)*45-175));
   



    //ImGui::Dummy(ImVec2(width, tabHight+(height/600)*50-50));
    ImGui::Dummy(ImVec2(width,  (height-tabHight)*0.099 +(600/height)*10-50 ));
   


    ImGui::Dummy(ImVec2(width*0.021-26, (height/600)));
    ImGui::SameLine();
    //ImPlot::BeginPlot("Spectrum", ImVec2(width*0.885+30, height*0.5+20 ), plotFlags);
    ImPlot::BeginPlot("Spectrum", ImVec2(width*0.79+30, height*0.59+28 ), plotFlags);
    ImPlot::SetupAxis(ImAxis_X1, "Hz", axisFlags | ImPlotAxisFlags_NoLabel);
    ImPlot::SetupAxis(ImAxis_Y1, "dB", axisFlags);
    ImPlot::SetupAxisLimits(ImAxis_X1, 0, 2400, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, -70, 0, ImGuiCond_Always);
    ImPlot::SetupLegend(ImPlotLocation_NorthWest, legendFlags);
    ImPlot::SetupFinish();
    ImPlot::PlotLine("", dataPsdOut.data(), 2400);
    ImPlot::EndPlot();
    ImGui::Dummy(ImVec2(width, (height/600)*19.5-20));
    // CutOff Knob
    // ---------------------------------------------------------------------------------------------- 
    float knobScale = (height/400)*49.5;
    float knobKonst = 0;
    ImGui::Dummy(ImVec2(width*0.045, 1));
    ImGui::SameLine();
    if (ImGuiKnobs::Knob("CutOff", &fCutOff, 0.0f, 100.0f, 0.1f, "%.1f%", ImGuiKnobVariant_Tick, knobScale+knobKonst))
    {
        if (ImGui::IsItemActivated())
            editParameter(0, true);

        // Max
        if (fCutOff > 100.0f)
            fCutOff = 100.0f;
        // Min
        if(fCutOff < 0.0f)
            fCutOff = 0.0f;

        setParameterValue(0, fCutOff);
    }
    if (ImGui::IsItemDeactivated())
        editParameter(0, false);
    // Resonance knob
    // -----------------------------------------------------------------------------------------------
    ImGui::SameLine();
    //ImGui::Dummy(ImVec2(width*0.25-15, 1));
    ImGui::Dummy(ImVec2(width*0.125-16, 1));
    ImGui::SameLine();
    if (ImGuiKnobs::Knob("Resonance", &fRes, 0.0f, 100.0f, 0.1f, "%.1f%", ImGuiKnobVariant_Tick, knobScale+knobKonst))
    {
        if (ImGui::IsItemActivated())
            editParameter(1, true);

        // Max
        if (fRes > 100.0f)
            fRes = 100.0f;

        // Min
        if(fRes < 0.0f)
            fRes = 0.0f;

        setParameterValue(1, fRes);
    }
    if (ImGui::IsItemDeactivated())
        editParameter(1, false);
    // Mode knob
    //------------------------------------------------------------------------------------------------
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(width*0.135-12, 1));
    ImGui::SameLine();
    if (ImGuiKnobs::Knob("Mode", &fMode, 1.0f, 4.0f, 1.0f, "%1.0f%", ImGuiKnobVariant_Stepped, knobScale+knobKonst))
    {
        if (ImGui::IsItemActivated())
            editParameter(2, true);

        // Max
        if (fMode > 4.0f)
            fMode = 4.0f;

        // Min
        if(fMode < 0.0f)
            fMode = 0.0f;

        setParameterValue(2, fMode);
    }
    if (ImGui::IsItemDeactivated())
        editParameter(2, false);
    
    // Wet knob
    //------------------------------------------------------------------------------------------------
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(width*0.125-16, 1));
    ImGui::SameLine();
    if (ImGuiKnobs::Knob("Wet", &fWet, 0.0f, 100.0f, 0.1f, "%.1f%", ImGuiKnobVariant_Tick, knobScale+knobKonst))
    {
        if (ImGui::IsItemActivated())
            editParameter(3, true);

        // Max
        if (fWet > 100.0f)
            fWet = 100.0f;

        // Min
        if(fWet < 0.0f)
            fWet = 0.0f;

        setParameterValue(3, fWet);
    }
    if (ImGui::IsItemDeactivated())
        editParameter(3, false);
    }
    // -----------------------------------------------------------------------------------------------
    ImGui::End();
    lastWidth = width;
    lastHeight = height;
}

// ---------------------------------------------------------------------------------------------------

UI* createUI()
{
    return new RobotHexedFilterUI();
}

// ---------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
