/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include <numeric>
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PolygonAlgorithm.h"

//==============================================================================
PolygonAudioProcessorEditor::PolygonAudioProcessorEditor(PolygonAudioProcessor& p, AudioProcessorValueTreeState& apvts, MidiKeyboardState& ks)
    : AudioProcessorEditor (&p), processor (p), parameters(apvts), keyboardState(ks), keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard), ringBuffer(p.getRingBuffer())
{
    //oscilloscope2D = std::make_unique<Oscilloscope2D>(ringBuffer);
    //oscilloscope2D->setVisible(true);
    //oscilloscope2D->start();
    //addChildComponent(*oscilloscope2D);

    GlyphArrangement glyph;
    glyph.addLineOfText(Font(32, Font::bold), "POLYGOGO SIM", 12, 38);
    glyph.createPath(titlePath);

    nameLabel.setFont(Font(16, Font::bold));
    nameLabel.setText("Dachun Sun", dontSendNotification);
    nameLabel.setColour(Label::textColourId, Colour(0xffdcc296));
    nameLabel.setJustificationType(Justification::bottomLeft);
    addAndMakeVisible(nameLabel);

    polygonOrderLabel.setText("Order", dontSendNotification);
    makeLabelUpperCase(polygonOrderLabel);
    addAndMakeVisible(polygonOrderLabel);
    addAndMakeVisible(polygonOrderSlider);
    polygonOrderSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 64, 32);
    polygonOrderSlider.setSliderStyle(Slider::LinearVertical);
    polygonOrderAttachment.reset(new SliderAttachment(parameters, "order", polygonOrderSlider));


    polygonRotationLabel.setText("Rotation", dontSendNotification);
    makeLabelUpperCase(polygonRotationLabel);
    addAndMakeVisible(polygonRotationLabel);
    addAndMakeVisible(polygonRotationSlider);
    polygonRotationSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 64, 32);
    polygonRotationSlider.setSliderStyle(Slider::LinearVertical);
    polygonRotationAttachment.reset(new SliderAttachment(parameters, "rotation", polygonRotationSlider));



    polygonTeethLabel.setText("Teeth", dontSendNotification);
    makeLabelUpperCase(polygonTeethLabel);
    addAndMakeVisible(polygonTeethLabel);
    addAndMakeVisible(polygonTeethSlider);
    polygonTeethSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 64, 32);
    polygonTeethSlider.setSliderStyle(Slider::LinearVertical);
    polygonTeethAttachment.reset(new SliderAttachment(parameters, "teeth", polygonTeethSlider));



    polygonFoldLabel.setText("Fold", dontSendNotification);
    makeLabelUpperCase(polygonFoldLabel);
    addAndMakeVisible(polygonFoldLabel);
    addAndMakeVisible(polygonFoldSlider);
    polygonFoldSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 64, 32);
    polygonFoldSlider.setSliderStyle(Slider::LinearVertical);
    polygonFoldAttachment.reset(new SliderAttachment(parameters, "fold", polygonFoldSlider));

    addAndMakeVisible(keyboardComponent);

    oversamplingLabel.setText("Oversampling", dontSendNotification);
    makeLabelUpperCase(oversamplingLabel);
    addAndMakeVisible(oversamplingLabel);
    addAndMakeVisible(oversamplingBox);
    oversamplingBox.addItem("1x", 1);
    oversamplingBox.addItem("2x", 2);
    oversamplingBox.addItem("4x", 4);
    oversamplingBox.addItem("8x", 8);
    oversamplingBox.addItem("16x", 16);
    oversamplingBox.setSelectedId(processor.getOversampleFactor());
    oversamplingBox.onChange = [&]() {
        processor.setOversampleFactor(oversamplingBox.getSelectedId());
    };

    voicesLabel.setText("Max Voices", dontSendNotification);
    makeLabelUpperCase(voicesLabel);
    addAndMakeVisible(voicesLabel);
    addAndMakeVisible(voicesSlider);
    voicesSlider.setRange(1, PolygonSynthAlgorithm::maxVoices, 1);
    voicesSlider.setValue(processor.getNumVoices());
    voicesSlider.onValueChange = [&]() {
        processor.setNumVoices(voicesSlider.getValue());
    };

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
    //oscilloscope2D->stop();
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

    g.setColour(Colour(0xff97b0c4));
    g.fillPath(titlePath);

    g.setColour(Colour(0xffdcc296));
    g.strokePath(titlePath, PathStrokeType(1.0f));


    Point<float> center{ getWidth() / 2.0f, getHeight() / 2.0f - 100 };
    Point<float> center2{ getWidth() / 2.0f + 200, getHeight() / 2.0f - 100 - 50 };
    g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    g.fillRect(center.getX() - 120, center.getY() - 120, 240.0f, 240.0f);

    g.setColour (Colours::white);
    //for (auto& edge : polygonEdges)
    //{
    //    auto p0 = edge.p0.rotatedAboutOrigin(polygonRotation) * (1.0f - polygonTeeth);
    //    auto p1 = edge.p1.rotatedAboutOrigin(polygonRotation) * (1.0f - edge.endpointWeight * polygonTeeth);
    //    g.drawLine(Line(100.0f * p0 + center2, 100.0f * p1 + center2), 2);
    //}
    //for (size_t i = 0; i < processor.getWavetableSize(); i++)
    //{
        //Point<float> p0{ waveX[i - 1], waveY[i - 1] }, p1{ waveX[i],waveY[i] };
        //g.drawLine(Line(100.0f * p0 + center, 100.0f * p1 + center), 2);
        //g.fillEllipse(100 * waveX[i] + center.getX(), 100 * waveY[i] + center.getY(), 1, 1);
    //}

    std::vector<Point<float>> vertices;
    for (int i = 0; i < 1024; i++)
    {
        vertices.push_back(PolygonSynthAlgorithm::getSample(i / 1024.0f, polygonOrderSlider.getValue(), polygonTeethSlider.getValue(), polygonFoldSlider.getValue(), polygonRotationSlider.getValue()));
    }
    for (int i = 1; i < vertices.size(); i++)
    {
        auto p = vertices[i], lastP = vertices[i - 1];
        g.fillEllipse(100 * p.getX() + center.getX(), 100 * -p.getY() + center.getY(), 1, 1);
        g.drawLine(100 * (i - 513) / 1200.0f + center2.getX(), 49 * -lastP.getX() + center2.getY(), 100 * (i - 512) / 1200.0 + center2.getX(), 49 * -p.getX() + center2.getY(), 1.0);
        g.drawLine(100 * (i - 513) / 1200.0f + center2.getX(), 49 * -lastP.getY() + center2.getY() + 102, 100 * (i - 512) / 1200.0 + center2.getX(), 49 * -p.getY() + center2.getY() + 102, 1.0);
    }
}

void PolygonAudioProcessorEditor::resized()
{
    auto area = getBounds();
    area.reduce(10, 10);

    auto titleAndSettingsArea = area.removeFromTop(70);
    auto titleArea = titleAndSettingsArea.removeFromLeft(titlePath.getBounds().getWidth() + 10 + 100).removeFromTop(32);
    titleArea.removeFromLeft(titlePath.getBounds().getWidth() + 10);
    nameLabel.setBounds(titleArea.removeFromLeft(100));

    // padding
    titleArea.removeFromLeft(titleArea.proportionOfWidth(0.5f));

    auto settingsArea = titleAndSettingsArea;
    auto settingsTopArea = settingsArea.removeFromTop(settingsArea.proportionOfHeight(0.5));

    auto outArea = settingsTopArea.removeFromRight(settingsTopArea.proportionOfWidth(0.5f));
    outGainLabel.setBounds(outArea.removeFromLeft(1.2f * getLabelWidth(outGainLabel)));
    outGainSlider.setBounds(outArea);
    oversamplingBox.setBounds(settingsTopArea.removeFromRight(settingsArea.proportionOfWidth(0.3f)));
    oversamplingLabel.setBounds(settingsTopArea.removeFromRight(getLabelWidth(oversamplingLabel)));

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
    
    auto middlePanel = area.withTrimmedTop(10).withTrimmedBottom(10);

    auto middleLeftPanel = middlePanel.removeFromLeft(middlePanel.proportionOfWidth(0.4));
    auto middleRightPanel = middlePanel;
    //oscilloscope2D->setBounds(middleRightPanel);
    auto middleLeftTopPanel = middleLeftPanel.removeFromTop(middleLeftPanel.proportionOfHeight(0.8));

    auto tmp = middleLeftTopPanel.proportionOfWidth(0.25);
    auto orderArea = middleLeftTopPanel.removeFromLeft(tmp);
    auto rotationArea = middleLeftTopPanel.removeFromLeft(tmp);
    auto teethArea = middleLeftTopPanel.removeFromLeft(tmp);
    auto foldArea = middleLeftTopPanel.removeFromLeft(tmp);

    setLabelAreaAboveCentered(polygonOrderLabel, orderArea);
    setLabelAreaAboveCentered(polygonRotationLabel, rotationArea);
    setLabelAreaAboveCentered(polygonTeethLabel, teethArea);
    setLabelAreaAboveCentered(polygonFoldLabel, foldArea);
    polygonOrderSlider.setBounds(orderArea);
    polygonRotationSlider.setBounds(rotationArea);
    polygonTeethSlider.setBounds(teethArea);
    polygonFoldSlider.setBounds(foldArea);
}

void PolygonAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &polygonOrderSlider)
    {
    }
}
