/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include <numeric>
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PolygonAudioProcessorEditor::PolygonAudioProcessorEditor(PolygonAudioProcessor& p, AudioProcessorValueTreeState& apvts, MidiKeyboardState& ks)
    : AudioProcessorEditor (&p), processor (p), parameters(apvts), keyboardState(ks), keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard), waveX(p.getWavetableX()), waveY(p.getWavetableY())
{
    addAndMakeVisible(polygonOrderSlider);
    polygonOrderSlider.setRange(2, 10);
    polygonOrderSlider.onValueChange = [&]() {
        calculatePolygon(vertices, polygonOrderSlider.getValue());
        processor.generateWavetable(processor.getWavetableSize(), waveX, waveY, vertices, polygonRotation, polygonTeeth, polygonFold);
        repaint();
    };

    addAndMakeVisible(polygonRotationSlider);
    polygonRotationSlider.setRange(0, MathConstants<float>::twoPi);
    polygonRotationSlider.onValueChange = [&]() {
        polygonRotation = polygonRotationSlider.getValue();
        processor.generateWavetable(processor.getWavetableSize(), waveX, waveY, vertices, polygonRotation, polygonTeeth, polygonFold);
        repaint();
    };

    addAndMakeVisible(polygonTeethSlider);
    polygonTeethSlider.setRange(0, 1);
    polygonTeethSlider.onValueChange = [&]() {
        polygonTeeth = polygonTeethSlider.getValue();
        processor.generateWavetable(processor.getWavetableSize(), waveX, waveY, vertices, polygonRotation, polygonTeeth, polygonFold);
        repaint();
    };
    //polygonTeethSlider.setValue(1);

    addAndMakeVisible(polygonFoldSlider);
    polygonFoldSlider.setRange(1, 2);
    polygonFoldSlider.onValueChange = [&]() {
        polygonFold = polygonFoldSlider.getValue();
        processor.generateWavetable(processor.getWavetableSize(), waveX, waveY, vertices, polygonRotation, polygonTeeth, polygonFold);
        repaint();
    };

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
    Point<float> center{ getWidth() / 2.0f, getHeight() / 2.0f - 100 };
    Point<float> center2{ getWidth() / 2.0f, getHeight() / 2.0f + 50 };
    g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    g.fillRect(center.getX() - 120, center.getY() - 120, 240.0f, 240.0f);

    g.setColour (Colours::white);
    //for (size_t i = 1; i < vertices.size(); i++)
    //{
    //    auto p0 = vertices[i - 1].rotatedAboutOrigin(polygonRotation);
    //    auto p1 = vertices[i].rotatedAboutOrigin(polygonRotation) * polygonTeeth;
    //    g.drawLine(Line(100.0f * p0 + center2, 100.0f * p1 + center2), 2);
    //}
    for (size_t i = 0; i < processor.getWavetableSize(); i++)
    {
        //Point<float> p0{ waveX[i - 1], waveY[i - 1] }, p1{ waveX[i],waveY[i] };
        //g.drawLine(Line(100.0f * p0 + center, 100.0f * p1 + center), 2);
        g.fillEllipse(100 * waveX[i] + center.getX(), 100 * waveY[i] + center.getY(), 1, 1);
    }
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
    polygonFoldSlider.setBounds(area.removeFromBottom(30));
    polygonTeethSlider.setBounds(area.removeFromBottom(30));
    polygonRotationSlider.setBounds(area.removeFromBottom(30));
    polygonOrderSlider.setBounds(area.removeFromBottom(30));
}

void PolygonAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &polygonOrderSlider)
    {
    }
}

void PolygonAudioProcessorEditor::calculatePolygon(std::vector<Point<float>>& vertices, float numVertices)
{
    vertices.clear();
    auto deltaAngle = MathConstants<float>::twoPi / numVertices;
    for (float i = 0; i < numVertices; i++)
    {
        vertices.push_back({ std::cos(deltaAngle * i), std::sin(deltaAngle * i) });
    }

    if (numVertices > 2)
    {
        auto arg_arccos = (1.0f - std::cosf(MathConstants<float>::twoPi / numVertices)) / 2.0f;
        auto x = std::sqrtf(1 - arg_arccos) / std::sinf(std::acosf(std::sqrtf(arg_arccos)) - (MathConstants<float>::twoPi / numVertices * std::floorf(numVertices)));
        vertices.push_back({ x, 0.0f });
    }
    else
    {
        vertices.push_back(vertices[0]);
    }
}