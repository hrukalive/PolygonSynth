/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PolygonVoice.h"
#include "PolygonSound.h"
#include "PolygonAlgorithm.h"
#include "hiir/PolyphaseIir2Designer.h"


AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        "outgain",
        "Out Gain",
        NormalisableRange<float>(0.0f,
            1.0f,
            0.0f,
            0.5f,
            false),
        0.5f,
        String(),
        AudioProcessorParameter::genericParameter,
        [](const float value, int /*maximumStringLength*/)
        {
            return String(Decibels::gainToDecibels(value), 2) +
                " dB";
        },
        [](const String& text)
        {
            return Decibels::decibelsToGain(text.getFloatValue());
        }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "attack",
        "Attack",
        NormalisableRange<float>(0.0f,
            20000.0f,
            0.0f,
            0.2f,
            false),
        1.0f,
        String(),
        AudioProcessorParameter::genericParameter,
        [](const float value, int /*maximumStringLength*/)
        {
            if (value >= 1000.0f)
                return String(value / 1000.0f, 2) + " s";
            return String(value, 2) + " ms";
        },
        [](const String& text)
        {
            if (text.endsWith(" s") ||
                (text.endsWith("s") && !text.endsWith("ms")))
            {
                return text.getFloatValue() * 1000.0f;
            }
            return text.getFloatValue();
        }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "decay",
        "Decay",
        NormalisableRange<float>(1.0f,
            60000.0f,
            0.0f,
            0.2f,
            false),
        600.0f,
        String(),
        AudioProcessorParameter::genericParameter,
        [](const float value, int /*maximumStringLength*/)
        {
            if (value >= 1000.0f)
                return String(value / 1000.0f, 2) + " s";
            return String(value, 2) + " ms";
        },
        [](const String& text)
        {
            if (text.endsWith(" s") ||
                (text.endsWith("s") && !text.endsWith("ms")))
            {
                return text.getFloatValue() * 1000.0f;
            }
            return text.getFloatValue();
        }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "sustain",
        "Sustain",
        NormalisableRange<float>(0.0f,
            1.0,
            0.0f,
            1.0f,
            false),
        1.0f,
        String(),
        AudioProcessorParameter::genericParameter,
        [](const float value, int /*maximumStringLength*/)
        {
            return String(Decibels::gainToDecibels(value), 2) +
                " dB";
        },
        [](const String& text)
        {
            return Decibels::decibelsToGain(text.getFloatValue());
        }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "release",
        "Release",
        NormalisableRange<float>(1.0f,
            60000.0f,
            0.0f,
            0.2f,
            false),
        50.0f,
        String(),
        AudioProcessorParameter::genericParameter,
        [](const float value, int /*maximumStringLength*/)
        {
            if (value >= 1000.0f)
                return String(value / 1000.0f, 2) + " s";
            return String(value, 2) + " ms";
        },
        [](const String& text)
        {
            if (text.endsWith(" s") ||
                (text.endsWith("s") && !text.endsWith("ms")))
            {
                return text.getFloatValue() * 1000.0f;
            }
            return text.getFloatValue();
        }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "order",
        "Order",
        NormalisableRange<float>(2.0f, 20.0f, 0.02f, 0.4f, false),
        3.0f,
        String(),
        AudioProcessorParameter::genericParameter,
        [](const float value, int /*maximumStringLength*/)
        {
            return String(value, 2);
        },
        [](const String& text)
        {
            return text.getFloatValue();
        }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "teeth",
        "Teeth",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f, false),
        0.0f,
        String(),
        AudioProcessorParameter::genericParameter,
        [](const float value, int /*maximumStringLength*/)
        {
            return String(int(value * 100)) + " %";
        },
        [](const String& text)
        {
            return text.getFloatValue() / 100.0f;
        }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "rotation",
        "Rottn",
        NormalisableRange<float>(0.0f, MathConstants<float>::twoPi, MathConstants<float>::twoPi / 360.0f, 1.0f, false),
        0.0f,
        String(),
        AudioProcessorParameter::genericParameter,
        [](const float value, int /*maximumStringLength*/)
        {
            return String(int(value / MathConstants<float>::twoPi * 360)) + String(CharPointer_UTF8("°"));
        },
        [](const String& text)
        {
            return text.getFloatValue() / 360.0f * MathConstants<float>::twoPi;
        }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "fold",
        "Fold",
        NormalisableRange<float>(1.0f, 10.0f, 0.05f, 0.4f, false),
        1.0f,
        String(),
        AudioProcessorParameter::genericParameter,
        [](const float value, int /*maximumStringLength*/)
        {
            return String(value, 2);
        },
        [](const String& text)
        {
            return text.getFloatValue();
        }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "fmratio",
        "Ratio",
        NormalisableRange<float>(0.1f, 10.0f, 0.05f, 0.4f, false),
        1.0f,
        String(),
        AudioProcessorParameter::genericParameter,
        [](const float value, int /*maximumStringLength*/)
        {
            return String(value, 2);
        },
        [](const String& text)
        {
            return text.getFloatValue();
        }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "fmamt",
        "Amt",
        NormalisableRange<float>(0.0f, 10.0f, 0.01f, 0.4f, false),
        0.0f,
        String(),
        AudioProcessorParameter::genericParameter,
        [](const float value, int /*maximumStringLength*/)
        {
            return String(value, 2);
        },
        [](const String& text)
        {
            return text.getFloatValue();
        }));
    return { params.begin(), params.end() };
}

//==============================================================================
PolygonAudioProcessor::PolygonAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
    ),
    parameters(*this, nullptr, Identifier("PolygonSynth"), createParameterLayout())
#endif
{
    Process::setPriority(Process::ProcessPriority::RealtimePriority);
    waveX.resize(nextWavetableSize, 0.0f);
    waveY.resize(nextWavetableSize, 0.0f);
    for (auto i = 0; i < numVoices; ++i)
    {
        synthesiser.addVoice(new PolygonVoice(parameters, envParams, waveX, waveY));
    }
    synthesiser.addSound(new PolygonSound());
}

PolygonAudioProcessor::~PolygonAudioProcessor()
{
}

//==============================================================================
const String PolygonAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PolygonAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PolygonAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PolygonAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PolygonAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PolygonAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PolygonAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PolygonAudioProcessor::setCurrentProgram (int index)
{
}

const String PolygonAudioProcessor::getProgramName (int index)
{
    return {};
}

void PolygonAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void PolygonAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synthesiser.setCurrentPlaybackSampleRate(sampleRate * oversampleFactor);

    oversampledBuffer.setSize(2, samplesPerBlock * maxOversampleFactor);
    oversampledBuffer.clear();

    double coefs[numCoeffs]{};
    hiir::PolyphaseIir2Designer::compute_coefs(coefs, 100.0, 0.1);
    downsamplerL.set_coefs(coefs);
    downsamplerL.clear_buffers();
    downsamplerR.set_coefs(coefs);
    downsamplerR.clear_buffers();

    double coefs2[numCoeffs2]{};
    hiir::PolyphaseIir2Designer::compute_coefs(coefs2, 100.0, 0.1);
    downsampler2L.set_coefs(coefs2);
    downsampler2L.clear_buffers();
    downsampler2R.set_coefs(coefs2);
    downsampler2R.clear_buffers();

    double coefs3[numCoeffs3]{};
    hiir::PolyphaseIir2Designer::compute_coefs(coefs3, 100.0, 0.1);
    downsampler3L.set_coefs(coefs3);
    downsampler3L.clear_buffers();
    downsampler3R.set_coefs(coefs3);
    downsampler3R.clear_buffers();

    double coefs4[numCoeffs4]{};
    hiir::PolyphaseIir2Designer::compute_coefs(coefs4, 100.0, 0.1);
    downsampler4L.set_coefs(coefs4);
    downsampler4L.clear_buffers();
    downsampler4R.set_coefs(coefs4);
    downsampler4R.clear_buffers();

    dcBlockerL.reset();
    dcBlockerR.reset();

    ringBuffer = std::make_shared<RingBuffer<float>>(2, samplesPerBlock * 10);
}

void PolygonAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PolygonAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PolygonAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateEnvParams();

    buffer.clear(0, 0, buffer.getNumSamples());
    buffer.clear(1, 0, buffer.getNumSamples());

    oversampledBuffer.clear(0, 0, oversampledBuffer.getNumSamples());
    oversampledBuffer.clear(1, 0, oversampledBuffer.getNumSamples());

    {
        int numSamplesPerOversampledBlock = samplesPerSubBlock * oversampleFactor;
        int totalSamples = buffer.getNumSamples() * oversampleFactor;
        int numWholeBlocks = totalSamples / numSamplesPerOversampledBlock;

        for (int blockNum = 0; blockNum < numWholeBlocks; ++blockNum)
        {
            const int startSample = blockNum * numSamplesPerOversampledBlock;

            keyboardState.processNextMidiBuffer(midiMessages, startSample, numSamplesPerOversampledBlock, true);
            synthesiser.renderNextBlock(oversampledBuffer, midiMessages, startSample, numSamplesPerOversampledBlock);
        }
        // in case the buffer didn't divide evenly.
        const int numSamplesLeft = totalSamples - (numWholeBlocks * numSamplesPerOversampledBlock);
        if (numSamplesLeft > 0)
        {
            const int startSample = totalSamples - numSamplesLeft;

            keyboardState.processNextMidiBuffer(midiMessages, startSample, numSamplesLeft, true);
            synthesiser.renderNextBlock(oversampledBuffer, midiMessages, startSample, numSamplesLeft);
        }
    }

    auto bufferWrite = buffer.getWritePointer(0);
    auto oversampleWrite = oversampledBuffer.getWritePointer(0);
    if (oversampleFactor >= 16)
    {
        //16x to 8x
        downsampler4L.process_block(oversampleWrite,
            oversampleWrite,
            buffer.getNumSamples() * 8);
    }
    if (oversampleFactor >= 8)
    {
        //8x to 4x
        downsampler3L.process_block(oversampleWrite,
            oversampleWrite,
            buffer.getNumSamples() * 4);
    }
    if (oversampleFactor >= 4)
    {
        //4x to 2x
        downsampler2L.process_block(oversampleWrite,
            oversampleWrite,
            buffer.getNumSamples() * 2);
    }
    if (oversampleFactor >= 2)
    {
        //2x to 1x
        downsamplerL.process_block(bufferWrite, // out
            oversampleWrite, // in
            buffer.getNumSamples());
    }
    else
    {
        //1x to 1x
        buffer.copyFrom(0, 0, oversampledBuffer, 0, 0, buffer.getNumSamples());
    }

    // filter dc offset
    for (int i = 0; i < buffer.getNumSamples(); ++i)
        bufferWrite[i] = dcBlockerL.pushSample(bufferWrite[i]);


    bufferWrite = buffer.getWritePointer(1);
    oversampleWrite = oversampledBuffer.getWritePointer(1);
    if (oversampleFactor >= 16)
    {
        //16x to 8x
        downsampler4R.process_block(oversampleWrite,
            oversampleWrite,
            buffer.getNumSamples() * 8);
    }
    if (oversampleFactor >= 8)
    {
        //8x to 4x
        downsampler3R.process_block(oversampleWrite,
            oversampleWrite,
            buffer.getNumSamples() * 4);
    }
    if (oversampleFactor >= 4)
    {
        //4x to 2x
        downsampler2R.process_block(oversampleWrite,
            oversampleWrite,
            buffer.getNumSamples() * 2);
    }
    if (oversampleFactor >= 2)
    {
        //2x to 1x
        downsamplerR.process_block(bufferWrite, // out
            oversampleWrite, // in
            buffer.getNumSamples());
    }
    else
    {
        //1x to 1x
        buffer.copyFrom(1, 0, oversampledBuffer, 1, 0, buffer.getNumSamples());
    }

    // filter dc offset
    for (int i = 0; i < buffer.getNumSamples(); ++i)
        bufferWrite[i] = dcBlockerR.pushSample(bufferWrite[i]);

    // todo smoothing
    buffer.applyGain(0, 0, buffer.getNumSamples(), parameters.getParameterAsValue("outgain").getValue());
    buffer.applyGain(1, 0, buffer.getNumSamples(), parameters.getParameterAsValue("outgain").getValue());

    ringBuffer->writeSamples(buffer, 0, buffer.getNumSamples());

    midiMessages.clear();
}

//==============================================================================
bool PolygonAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PolygonAudioProcessor::createEditor()
{
    return new PolygonAudioProcessorEditor (*this, parameters, keyboardState);
}

//==============================================================================
void PolygonAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    jassert(waveX.size() == waveY.size());

    const auto state = parameters.copyState();
    const auto xml(state.createXml());
    xml->setAttribute("maxVoices", numVoices);
    xml->setAttribute("oversampleFactor", oversampleFactor);
    xml->setAttribute("wavetableSize", static_cast<int>(waveX.size()));
    copyXmlToBinary(*xml, destData);
}

void PolygonAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    const auto xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
    {
        if (xmlState->hasAttribute("maxVoices"))
        {
            setNumVoices(xmlState->getIntAttribute("maxVoices", 10));
            xmlState->removeAttribute("maxVoices");
        }
        if (xmlState->hasAttribute("oversampleFactor"))
        {
            setOversampleFactor(xmlState->getIntAttribute("oversampleFactor", 2));
            xmlState->removeAttribute("oversampleFactor");
        }
        if (xmlState->hasAttribute("wavetableSize"))
        {
            setWavetableSize(xmlState->getIntAttribute("wavetableSize", 1024));
            xmlState->removeAttribute("wavetableSize");
        }
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            parameters.replaceState(ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
void PolygonAudioProcessor::updateEnvParams()
{
    envParams.attack = *parameters.getRawParameterValue("attack") / 1000.0f;
    envParams.decay = *parameters.getRawParameterValue("decay") / 1000.0f;
    envParams.sustain = *parameters.getRawParameterValue("sustain");
    envParams.release = *parameters.getRawParameterValue("release") / 1000.0f;
}

void PolygonAudioProcessor::setNumVoices(int newNumVoices)
{
    numVoices = newNumVoices;
    const auto synthNumVoices = synthesiser.getNumVoices();
    if (synthNumVoices == numVoices)
    {
        return;
    }
    if (synthNumVoices < numVoices)
    {
        for (auto i = synthNumVoices; i < numVoices; ++i)
        {
            synthesiser.addVoice(new PolygonVoice(parameters, envParams, waveX, waveY));
        }
        jassert(numVoices == synthesiser.getNumVoices());
        return;
    }
    // synthNumVoices > numVoices
    for (auto i = synthNumVoices; i > numVoices; --i)
    {
        synthesiser.removeVoice(i - 1);
    }
    jassert(numVoices == synthesiser.getNumVoices());
}

int PolygonAudioProcessor::getNumVoices()
{
    jassert(numVoices == synthesiser.getNumVoices());
    return numVoices;
}

void PolygonAudioProcessor::setOversampleFactor(int newOversampleFactor)
{
    oversampleFactor = newOversampleFactor;
    synthesiser.setCurrentPlaybackSampleRate(getSampleRate() * oversampleFactor);
}

//==============================================================================
void PolygonAudioProcessor::setWavetableSize(int newWavetableSize)
{
    nextWavetableSize = newWavetableSize;
}


// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PolygonAudioProcessor();
}

std::pair<std::vector<float>, std::vector<float>> PolygonAudioProcessor::generateWavetable(size_t wavetableSize)
{
    std::vector<float> waveX(wavetableSize, 0.0f);
    std::vector<float> waveY(wavetableSize, 0.0f);

    for (size_t i = 0; i < wavetableSize; i++)
    {
        const auto value = PolygonSynthAlgorithm::getSample(
            (float)i / (float)wavetableSize,
            *parameters.getRawParameterValue("order"),
            *parameters.getRawParameterValue("teeth"),
            *parameters.getRawParameterValue("fold"),
            *parameters.getRawParameterValue("rotation"));
        waveX[i] = value.getX();
        waveY[i] = value.getY();
    }
    return std::make_pair(waveX, waveY);
}
