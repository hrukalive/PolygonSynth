/*
  ==============================================================================

    TuningEditor.h
    Created: 16 Apr 2020 2:39:10pm
    Author:  bowen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>`

class TuningEditor : public Component
{
public:
    TuningEditor(AudioProcessorValueTreeState&);
    ~TuningEditor() = default;

    //==============================================================================
    void makeLabelUpperCase(Label& label);
    static void setLabelAreaAboveCentered(Label& label, Rectangle<int>& labelArea);
    static int getLabelWidth(Label& label);

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;
    AudioProcessorValueTreeState& parameters;

    Label rootLabel;
    Label presetLabel;
    ComboBox rootBox;
    ComboBox presetBox;

    String centShiftParamNames[12]{ "centShiftOfC", "centShiftOfC#",
        "centShiftOfD", "centShiftOfD#", "centShiftOfE", "centShiftOfF", "centShiftOfF#",
        "centShiftOfG", "centShiftOfG#", "centShiftOfA", "centShiftOfA#", "centShiftOfB", };
    Label noteLabels[12];
    Slider noteSliders[12];
    std::unique_ptr<SliderAttachment> sliderAttachments[12];

    int presets[4][12]{
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 12, 4, 16, -14, -2, -17, 2, 14, -16, 18, -12},
        {0, -10, 4, -6, 8, -2, -12, 2, -8, 6, -4, 10},
        {0, -24, -7, 10, -14, 3, -21, -3, -27, -10, 7, -17}
    };

    TextButton okBtn{ "Done" };
};
