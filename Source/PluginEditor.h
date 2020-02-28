/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PolygonDisplay.h"

//==============================================================================
/**
*/
class PolygonAudioProcessorEditor : public AudioProcessorEditor,
    public Slider::Listener
{
public:
    PolygonAudioProcessorEditor(PolygonAudioProcessor&, AudioProcessorValueTreeState&, MidiKeyboardState&);
    ~PolygonAudioProcessorEditor();

    //==============================================================================
    void makeLabelUpperCase(Label& label);
    void setupAdsrControl(Label& label, Slider& slider,
        std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>& attachment,
        const String& labelText, const String& parameterId);
    static void setLabelAreaAboveCentered(Label& label, Rectangle<int>& labelArea);
    static int getLabelWidth(Label& label);

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void sliderValueChanged(Slider* slider) override;

    void stopDrawing() { display->stopDrawing(); }
    void startDrawing() { display->startDrawing(); }
    void updateBlocksize(float bs) { display->updateBlocksize(bs); }

private:
    using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;

    PolygonAudioProcessor& processor;
    AudioProcessorValueTreeState& parameters;

    MidiKeyboardState& keyboardState;
    MidiKeyboardComponent keyboardComponent;

    Label voicesLabel;
    Slider voicesSlider;

    Label oversamplingLabel;
    ComboBox oversamplingBox;

    Label attackLabel;
    Slider attackSlider;
    std::unique_ptr<SliderAttachment> attackAttachment;

    Label decayLabel;
    Slider decaySlider;
    std::unique_ptr<SliderAttachment> decayAttachment;

    Label sustainLabel;
    Slider sustainSlider;
    std::unique_ptr<SliderAttachment> sustainAttachment;

    Label releaseLabel;
    Slider releaseSlider;
    std::unique_ptr<SliderAttachment> releaseAttachment;

    Label outGainLabel;
    Slider outGainSlider;
    std::unique_ptr<SliderAttachment> outGainAttachment;

    Label polygonOrderLabel;
    Slider polygonOrderSlider;
    std::unique_ptr<SliderAttachment> polygonOrderAttachment;

    Label polygonTeethLabel;
    Slider polygonTeethSlider;
    std::unique_ptr<SliderAttachment> polygonTeethAttachment;

    Label polygonFoldLabel;
    Slider polygonFoldSlider;
    std::unique_ptr<SliderAttachment> polygonFoldAttachment;

    Label polygonRotationLabel;
    Slider polygonRotationSlider;
    std::unique_ptr<SliderAttachment> polygonRotationAttachment;

    Label polygonFmRatioLabel;
    Slider polygonFmRatioSlider;
    std::unique_ptr<SliderAttachment> polygonFmRatioAttachment;

    Label polygonFmAmtLabel;
    Slider polygonFmAmtSlider;
    std::unique_ptr<SliderAttachment> polygonFmAmtAttachment;

    Label nameLabel;
    Path titlePath;

    std::unique_ptr<PolygonDisplay> display;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolygonAudioProcessorEditor)
};
