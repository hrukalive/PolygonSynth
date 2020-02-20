/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PolygonAudioProcessorEditor::PolygonAudioProcessorEditor(PolygonAudioProcessor& p, AudioProcessorValueTreeState& apvts, MidiKeyboardState& ks)
    : AudioProcessorEditor (&p), processor (p), parameters(apvts), keyboardState(ks), keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard)
{
    addAndMakeVisible(polygonSlider);
    polygonSlider.setRange(3, 10);
    polygonSlider.addListener(this);

    addAndMakeVisible(keyboardComponent);

    outGainLabel.setText("Out", dontSendNotification);
    makeLabelUpperCase(outGainLabel);
    addAndMakeVisible(outGainLabel);
    addAndMakeVisible(outGainSlider);
    outGainSlider.setSliderStyle(Slider::Rotary);
    outGainAttachment.reset(new SliderAttachment(parameters, "outgain", outGainSlider));

    setupAdsrControl(attackLabel, attackSlider, attackAttachment, "Attack", "attack");
    setupAdsrControl(decayLabel, decaySlider, decayAttachment, "Decay", "decay");
    setupAdsrControl(sustainLabel, sustainSlider, sustainAttachment, "Sustain", "sustain");
    setupAdsrControl(releaseLabel, releaseSlider, releaseAttachment, "Release", "release");

    setResizable(true, true);
    setResizeLimits(256, 256, 2048, 2048);
    setSize(885, 720);
}

PolygonAudioProcessorEditor::~PolygonAudioProcessorEditor()
{
}

void PolygonAudioProcessorEditor::makeLabelUpperCase(Label& label)
{
    label.setText(label.getText().toUpperCase(), dontSendNotification);
}

void PolygonAudioProcessorEditor::setupAdsrControl(Label& label, Slider& slider,
    std::unique_ptr<SliderAttachment>& attachment,
    const String& labelText, const String& parameterId)
{
    label.setText(labelText, dontSendNotification);
    makeLabelUpperCase(label);
    addAndMakeVisible(label);

    slider.setTextBoxStyle(Slider::TextBoxAbove, false, 64, 32);
    slider.setSliderStyle(Slider::LinearBar);
    addAndMakeVisible(slider);

    attachment.reset(new SliderAttachment(parameters, parameterId, slider));
}

//==============================================================================
void PolygonAudioProcessorEditor::setLabelAreaAboveCentered(Label& label, Rectangle<int>& labelArea)
{
    label.setBounds(
        labelArea.removeFromTop(16).withSizeKeepingCentre(
            6 + label.getFont().getStringWidth(label.getText()), 16));
}

int PolygonAudioProcessorEditor::getLabelWidth(Label& label)
{
    return label.getFont().getStringWidth(label.getText());
}

void PolygonAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
    for (auto i = 0; vertices.size() > 1 && i < vertices.size() - 1; i++)
        g.drawLine(vertices[i].getX(), vertices[i].getY(), vertices[i + 1].getX(), vertices[i + 1].getY(), 2);
}

void PolygonAudioProcessorEditor::resized()
{
    auto area = getBounds();
    area.reduce(10, 10);

    auto adsrBounds = area.removeFromTop(area.proportionOfHeight(0.1f));
    auto adsrWidth = adsrBounds.getWidth() / 4.0f;

    auto attackArea = adsrBounds.removeFromLeft(adsrWidth);
    auto decayArea = adsrBounds.removeFromLeft(adsrWidth);
    auto sustainArea = adsrBounds.removeFromLeft(adsrWidth);
    auto releaseArea = adsrBounds.removeFromLeft(adsrWidth);

    setLabelAreaAboveCentered(attackLabel, attackArea);
    setLabelAreaAboveCentered(decayLabel, decayArea);
    setLabelAreaAboveCentered(sustainLabel, sustainArea);
    setLabelAreaAboveCentered(releaseLabel, releaseArea);

    attackSlider.setBounds(attackArea);
    decaySlider.setBounds(decayArea);
    sustainSlider.setBounds(sustainArea);
    releaseSlider.setBounds(releaseArea);

    keyboardComponent.setBounds(area.removeFromBottom(100));
    polygonSlider.setBounds(area.removeFromBottom(30));
}

void PolygonAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &polygonSlider)
    {
        calculatePolygon(vertices, polygonSlider.getValue(), { getWidth() / 2.0f, getHeight() / 2.0f }, 100);
        repaint();
    }
}

void PolygonAudioProcessorEditor::calculatePolygon(std::vector<Point<float>>& vertices, size_t numVertices, Point<float> center, float radius)
{
    auto deltaAngle = MathConstants<float>::twoPi / numVertices;
    for (auto i = 0; i < numVertices; i++)
    {
        vertices.push_back({ radius * std::cos(deltaAngle * i) + center.getX(), radius * std::sin(deltaAngle * i) + center.getY() });
    }
}
