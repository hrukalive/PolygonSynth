/*
  ==============================================================================

    TuningEditor.cpp
    Created: 16 Apr 2020 2:39:10pm
    Author:  bowen

  ==============================================================================
*/

#include "TuningEditor.h"

TuningEditor::TuningEditor(AudioProcessorValueTreeState& apvts) :
    parameters(apvts)
{
    for (int i = 0; i < 12; i++)
    {
        rootBox.addItem(centShiftParamNames[i].substring(11), i + 1);
        noteLabels[i].setText(centShiftParamNames[i].substring(11), dontSendNotification);
        makeLabelUpperCase(noteLabels[i]);
        addAndMakeVisible(noteLabels[i]);
        addAndMakeVisible(noteSliders[i]);
        sliderAttachments[i].reset(new SliderAttachment(parameters, centShiftParamNames[i], noteSliders[i]));
    }

    rootLabel.setText("Root", dontSendNotification);
    makeLabelUpperCase(rootLabel);
    addAndMakeVisible(rootLabel);

    presetLabel.setText("Preset", dontSendNotification);
    makeLabelUpperCase(presetLabel);
    addAndMakeVisible(presetLabel);

    rootBox.setSelectedId(1, false);
    addAndMakeVisible(rootBox);
    rootBox.onChange = [&]() {
        if (presetBox.getSelectedId() != 0)
        {
            for (int i = 0; i < 12; i++)
            {
                auto* param = parameters.getParameter(centShiftParamNames[i]);
                param->setValue(param->convertTo0to1(
                    presets[presetBox.getSelectedId() - 1][((i - (rootBox.getSelectedId() - 1)) % 12 + 12) % 12] -
                    presets[presetBox.getSelectedId() - 1][((9 - (rootBox.getSelectedId() - 1)) % 12 + 12) % 12]));
            }
        }
    };

    presetBox.addItem("12-TET", 1);
    presetBox.addItem("Just", 2);
    presetBox.addItem("Pythagorean", 3);
    presetBox.addItem("1/4 Comma Meantone", 4);
    presetBox.addItem("Werckmeister III", 5);
    addAndMakeVisible(presetBox);
    presetBox.onChange = [&]() {
        for (int i = 0; i < 12; i++)
        {
            auto* param = parameters.getParameter(centShiftParamNames[i]);
            param->setValue(param->convertTo0to1(
                presets[presetBox.getSelectedId() - 1][((i - (rootBox.getSelectedId() - 1)) % 12 + 12) % 12] -
                presets[presetBox.getSelectedId() - 1][((9 - (rootBox.getSelectedId() - 1)) % 12 + 12) % 12]));
        }
    };

    okBtn.onClick = [&]() {
        this->getTopLevelComponent()->exitModalState(0);
    };
    addAndMakeVisible(okBtn);

    setSize(300, 400);
}

//==============================================================================
void TuningEditor::makeLabelUpperCase(Label& label)
{
    label.setText(label.getText().toUpperCase(), dontSendNotification);
}

void TuningEditor::setLabelAreaAboveCentered(Label& label, Rectangle<int>& labelArea)
{
    label.setBounds(
        labelArea.removeFromTop(16).withSizeKeepingCentre(
            6 + label.getFont().getStringWidth(label.getText()), 16));
}

int TuningEditor::getLabelWidth(Label& label)
{
    return label.getFont().getStringWidth(label.getText());
}

//==============================================================================
void TuningEditor::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void TuningEditor::resized()
{
    auto area = getBounds();
    area.reduce(10, 10);
    auto tmpWidth = area.proportionOfWidth(0.8);

    auto tmp = area.removeFromTop(24);
    rootBox.setBounds(tmp.removeFromRight(tmpWidth));
    rootLabel.setBounds(tmp);
    area.removeFromTop(4);

    tmp = area.removeFromTop(24);
    presetBox.setBounds(tmp.removeFromRight(tmpWidth));
    presetLabel.setBounds(tmp);
    area.removeFromTop(4);

    okBtn.setBounds(area.removeFromBottom(24));
    area.removeFromBottom(4);

    auto tmpHeight = area.proportionOfHeight(1 / 12.0f);
    tmpWidth = area.proportionOfWidth(0.9);
    for (int i = 0; i < 12; i++)
    {
        auto tmp = area.removeFromTop(tmpHeight);
        noteSliders[i].setBounds(tmp.removeFromRight(tmpWidth));
        noteLabels[i].setBounds(tmp);
    }
}