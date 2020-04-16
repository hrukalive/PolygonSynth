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
#include "TuningEditor.h"

//==============================================================================
PolygonAudioProcessorEditor::PolygonAudioProcessorEditor(PolygonAudioProcessor& p, AudioProcessorValueTreeState& apvts, MidiKeyboardState& ks)
    : AudioProcessorEditor (&p), processor (p), parameters(apvts), keyboardState(ks), keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard)
{
    display = std::make_unique<PolygonDisplay>(p);
    addAndMakeVisible(*display);

    GlyphArrangement glyph;
    glyph.addLineOfText(Font(32, Font::bold), "POLYGOGO SIM", 12, 38);
    glyph.createPath(titlePath);

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


    polygonFmRatioLabel.setText("FM Ratio", dontSendNotification);
    makeLabelUpperCase(polygonFmRatioLabel);
    addAndMakeVisible(polygonFmRatioLabel);
    addAndMakeVisible(polygonFmRatioSlider);
    polygonFmRatioSlider.setTextBoxStyle(Slider::TextBoxRight, false, 64, 32);
    polygonFmRatioAttachment.reset(new SliderAttachment(parameters, "fmratio", polygonFmRatioSlider));


    polygonFmAmtLabel.setText("FM Amt", dontSendNotification);
    makeLabelUpperCase(polygonFmAmtLabel);
    addAndMakeVisible(polygonFmAmtLabel);
    addAndMakeVisible(polygonFmAmtSlider);
    polygonFmAmtSlider.setTextBoxStyle(Slider::TextBoxRight, false, 64, 32);
    polygonFmAmtAttachment.reset(new SliderAttachment(parameters, "fmamt", polygonFmAmtSlider));


    freqOfALabel.setText("A4 Freq", dontSendNotification);
    makeLabelUpperCase(freqOfALabel);
    addAndMakeVisible(freqOfALabel);
    addAndMakeVisible(freqOfASlider);
    freqOfASlider.setTextBoxStyle(Slider::TextBoxRight, false, 64, 32);
    freqOfAAttachment.reset(new SliderAttachment(parameters, "freqOfA", freqOfASlider));

    tuningBtn.onClick = [&]() {
        DialogWindow::LaunchOptions dialogOption;

        dialogOption.dialogTitle = "Tuning";
        dialogOption.dialogBackgroundColour = LookAndFeel::getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId);
        dialogOption.escapeKeyTriggersCloseButton = false;
        dialogOption.useNativeTitleBar = false;
        dialogOption.resizable = true;
        dialogOption.content.setOwned(new TuningEditor(apvts));
        dialogOption.launchAsync();
    };
    addAndMakeVisible(tuningBtn);

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

    outGainLabel.setText("Out", dontSendNotification);
    makeLabelUpperCase(outGainLabel);
    addAndMakeVisible(outGainLabel);
    addAndMakeVisible(outGainSlider);
    outGainAttachment.reset(new SliderAttachment(parameters, "outgain", outGainSlider));

    velGammaLabel.setText("Vel.S", dontSendNotification);
    makeLabelUpperCase(velGammaLabel);
    addAndMakeVisible(velGammaLabel);
    addAndMakeVisible(velGammaSlider);
    velGammaAttachment.reset(new SliderAttachment(parameters, "velgamma", velGammaSlider));

    setupAdsrControl(attackLabel, attackSlider, attackAttachment, "Attack", "attack");
    setupAdsrControl(decayLabel, decaySlider, decayAttachment, "Decay", "decay");
    setupAdsrControl(sustainLabel, sustainSlider, sustainAttachment, "Sustain", "sustain");
    setupAdsrControl(releaseLabel, releaseSlider, releaseAttachment, "Release", "release");

    setResizable(true, true);
    setResizeLimits(600, 500, 2048, 2048);
    setSize(885, 720);
}

PolygonAudioProcessorEditor::~PolygonAudioProcessorEditor()
{
    display = nullptr;
    processor.editorClosed();
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
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour(Colour(0xffdcc296));
    g.fillPath(titlePath);
}

void PolygonAudioProcessorEditor::resized()
{
    auto area = getBounds();
    area.reduce(10, 10);

    auto settingsArea = area.removeFromTop(40).withTrimmedLeft(int(titlePath.getBounds().getWidth()));

    auto tmp = settingsArea.proportionOfWidth(0.31);
    auto outGainArea = settingsArea.removeFromRight(tmp);
    auto velGammaArea = settingsArea.removeFromRight(tmp);
    auto oversamplingArea = settingsArea.removeFromRight(tmp);
    outGainLabel.setBounds(outGainArea.removeFromLeft(int(1.1f * getLabelWidth(outGainLabel))));
    outGainSlider.setBounds(outGainArea);
    velGammaLabel.setBounds(velGammaArea.removeFromLeft(int(1.1f * getLabelWidth(velGammaLabel))));
    velGammaSlider.setBounds(velGammaArea);
    oversamplingLabel.setBounds(oversamplingArea.removeFromLeft(int(1.1f * getLabelWidth(oversamplingLabel))));
    oversamplingBox.setBounds(oversamplingArea);

    auto adsrBounds = area.removeFromTop(area.proportionOfHeight(0.1f));
    auto adsrWidth = int(adsrBounds.getWidth() / 4);

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
    auto middleRightPanel = middlePanel.withTrimmedLeft(4);
    display->setBounds(middleRightPanel);
    auto middleLeftTopPanel = middleLeftPanel.removeFromTop(middleLeftPanel.proportionOfHeight(0.7));
    auto middleLeftBottomPanel = middleLeftPanel.withTrimmedTop(4);

    tmp = middleLeftTopPanel.proportionOfWidth(0.25);
    auto orderArea = middleLeftTopPanel.removeFromLeft(tmp);
    auto teethArea = middleLeftTopPanel.removeFromLeft(tmp);
    auto foldArea = middleLeftTopPanel.removeFromLeft(tmp);
    auto rotationArea = middleLeftTopPanel.removeFromLeft(tmp);

    setLabelAreaAboveCentered(polygonOrderLabel, orderArea);
    setLabelAreaAboveCentered(polygonTeethLabel, teethArea);
    setLabelAreaAboveCentered(polygonFoldLabel, foldArea);
    setLabelAreaAboveCentered(polygonRotationLabel, rotationArea);
    polygonOrderSlider.setBounds(orderArea);
    polygonTeethSlider.setBounds(teethArea);
    polygonFoldSlider.setBounds(foldArea);
    polygonRotationSlider.setBounds(rotationArea);

    tmp = middleLeftBottomPanel.proportionOfHeight(0.33);
    auto fmRatioArea = middleLeftBottomPanel.removeFromTop(tmp);
    auto fmAmtArea = middleLeftBottomPanel.removeFromTop(tmp);
    auto tuningArea = middleLeftBottomPanel.removeFromTop(tmp);
    tmp = jmax(getLabelWidth(polygonFmRatioLabel), getLabelWidth(polygonFmAmtLabel), getLabelWidth(freqOfALabel));
    polygonFmRatioLabel.setBounds(fmRatioArea.removeFromLeft(tmp));
    polygonFmRatioSlider.setBounds(fmRatioArea);
    polygonFmAmtLabel.setBounds(fmAmtArea.removeFromLeft(tmp));
    polygonFmAmtSlider.setBounds(fmAmtArea);
    freqOfALabel.setBounds(tuningArea.removeFromLeft(tmp));
    tuningBtn.setBounds(tuningArea.removeFromRight(80));
    tuningArea.removeFromRight(4);
    freqOfASlider.setBounds(tuningArea);
}
