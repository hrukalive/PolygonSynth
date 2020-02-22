/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

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
    static void calculatePolygon(std::vector<Point<float>>& vertices, float numVertices);

private:
    using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;

    PolygonAudioProcessor& processor;
    AudioProcessorValueTreeState& parameters;

    MidiKeyboardState& keyboardState;
    MidiKeyboardComponent keyboardComponent;

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

    Label polygonRotationLabel;
    Slider polygonRotationSlider;

    Label polygonTeethLabel;
    Slider polygonTeethSlider;

    Label polygonFoldLabel;
    Slider polygonFoldSlider;

    float polygonRotation = 0, polygonTeeth = 1, polygonFold = 1;

    std::vector<Point<float>> vertices = std::vector<Point<float>>(3, { 0.0f, 0.0f });
    std::vector<float> &waveX, &waveY;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolygonAudioProcessorEditor)
};
