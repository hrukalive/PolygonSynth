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
#include "hiir/PolyphaseIir2Designer.h"

struct PolygonSynthConstants
{
public:
    static constexpr int maxVoices{ 20 };
};


AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>("outgain",
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
    params.push_back(std::make_unique<AudioParameterFloat>("attack",
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
    params.push_back(std::make_unique<AudioParameterFloat>("decay",
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
    params.push_back(std::make_unique<AudioParameterFloat>("sustain",
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
    params.push_back(std::make_unique<AudioParameterFloat>("release",
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
    wavetable.resize(nextWavetableSize, 0.0f);
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

    oversampledBuffer.setSize(1, samplesPerBlock * maxOversampleFactor);
    oversampledBuffer.clear();

    double coefs[numCoeffs]{};
    hiir::PolyphaseIir2Designer::compute_coefs(coefs, 100.0, 0.1);
    downsampler.set_coefs(coefs);
    downsampler.clear_buffers();

    double coefs2[numCoeffs2]{};
    hiir::PolyphaseIir2Designer::compute_coefs(coefs2, 100.0, 0.1);
    downsampler2.set_coefs(coefs2);
    downsampler2.clear_buffers();

    double coefs3[numCoeffs3]{};
    hiir::PolyphaseIir2Designer::compute_coefs(coefs3, 100.0, 0.1);
    downsampler3.set_coefs(coefs3);
    downsampler3.clear_buffers();

    double coefs4[numCoeffs4]{};
    hiir::PolyphaseIir2Designer::compute_coefs(coefs4, 100.0, 0.1);
    downsampler4.set_coefs(coefs4);
    downsampler4.clear_buffers();

    dcBlocker.reset();
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

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateEnvParams();

    buffer.clear(0, 0, buffer.getNumSamples());

    oversampledBuffer.clear(0, 0, oversampledBuffer.getNumSamples());

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
        downsampler4.process_block(oversampleWrite,
            oversampleWrite,
            buffer.getNumSamples() * 8);
    }
    if (oversampleFactor >= 8)
    {
        //8x to 4x
        downsampler3.process_block(oversampleWrite,
            oversampleWrite,
            buffer.getNumSamples() * 4);
    }
    if (oversampleFactor >= 4)
    {
        //4x to 2x
        downsampler2.process_block(oversampleWrite,
            oversampleWrite,
            buffer.getNumSamples() * 2);
    }
    if (oversampleFactor >= 2)
    {
        //2x to 1x
        downsampler.process_block(bufferWrite, // out
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
    {
        bufferWrite[i] = dcBlocker.pushSample(bufferWrite[i]);
    }

    // todo smoothing
    buffer.applyGain(0, 0, buffer.getNumSamples(), parameters.getParameterAsValue("outgain").getValue());

    // copy the processed channel to all the other channels
    for (auto i = 1; i < totalNumOutputChannels; ++i)
        buffer.copyFrom(i, 0, buffer, 0, 0, buffer.getNumSamples());

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
    const auto state = parameters.copyState();
    const auto xml(state.createXml());
    xml->setAttribute("maxVoices", numVoices);
    xml->setAttribute("oversampleFactor", oversampleFactor);
    xml->setAttribute("wavetableSize", static_cast<int>(wavetable.size()));
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
            setOversampleFactor(xmlState->getIntAttribute("oversampleFactor", 8));
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

void PolygonAudioProcessor::generateWavetable(
    size_t wavetableSize,
    std::vector<float>& waveX,
    std::vector<float>& waveY,
    const std::vector<Point<float>> vertices,
    float rotation, float teeth, float fold)
{
    waveX.resize(wavetableSize, 0.0f);
    waveY.resize(wavetableSize, 0.0f);

    std::vector<float> lengthArray;
    float totalLength = 0;
    lengthArray.push_back(0.0f);
    for (size_t i = 1; i < vertices.size(); i++)
    {
        auto dist = (teeth * vertices[i]).getDistanceFrom(vertices[i - 1]);
        totalLength += dist;
        lengthArray.push_back(dist + lengthArray[lengthArray.size() - 1]);
    }
    auto deltaL = totalLength / (float)wavetableSize;

    float currentPos = 0.0f;
    size_t nextToLook = 1;

    auto p0 = vertices[0].rotatedAboutOrigin(rotation), p1 = vertices[1].rotatedAboutOrigin(rotation) * teeth;
    auto length = lengthArray[1] - lengthArray[0], offset = lengthArray[0];
    Line<float> line(p0, p1);
    for (size_t i = 0; i < wavetableSize; i++)
    {
        if (currentPos > lengthArray[nextToLook])
        {
            nextToLook += 1;
            // length = lengthArray[nextToLook] - lengthArray[nextToLook - 1];
            offset = lengthArray[nextToLook - 1];
            p0 = vertices[nextToLook - 1].rotatedAboutOrigin(rotation);
            p1 = vertices[nextToLook].rotatedAboutOrigin(rotation) * teeth;

            line = Line(p0, p1);
        }
        auto t = currentPos - offset;
        auto val = line.getPointAlongLine(t);
        waveX[i] = val.getX() * fold;
        while (waveX[i] > 1)
            waveX[i] -= 2.0f;
        while (waveX[i] < -1)
            waveX[i] += 2.0f;
        waveY[i] = val.getY() * fold;
        while (waveY[i] > 1)
            waveY[i] -= 2.0f;
        while (waveY[i] < -1)
            waveY[i] += 2.0f;
        currentPos += deltaL;
    }
}
