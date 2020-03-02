/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "hiir/Downsampler2xFpu.h"
#include "DcBlocker.h"
#include "RingBuffer.h"
#include "PolygonAlgorithm.h"

class PolygonAudioProcessorEditor;

//==============================================================================
/**
*/
class PolygonAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    PolygonAudioProcessor();
    ~PolygonAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int /*index*/) override;
    const String getProgramName (int /*index*/) override;
    void changeProgramName (int /*index*/, const String& /*newName*/) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    inline std::pair<std::vector<float>, std::vector<float>> generateWavetable(size_t wavetableSize)
    {
        std::vector<float> waveX(wavetableSize, 0.0f);
        std::vector<float> waveY(wavetableSize, 0.0f);

        for (size_t i = 0; i < wavetableSize; i++)
        {
            auto value = PolygonSynthAlgorithm::getSample((float)i / (float)wavetableSize, 0, *orderParameter, *teethParameter, *foldParameter);
            waveX[i] = value.getX();
            waveY[i] = value.getY();
        }
        return std::make_pair(waveX, waveY);
    }

    //==============================================================================
    void setNumVoices(int newNumVoices);
    int getNumVoices();
    void setOversampleFactor(int newOversampleFactor);
    int getOversampleFactor() const { return oversampleFactor; }
    std::shared_ptr<RingBuffer<float>> getRingBuffer() { return ringBuffer; }
    AudioProcessorValueTreeState& getParameters() { return parameters; }

    void editorClosed() { editor = nullptr; }

private:
    AudioProcessorValueTreeState parameters;
    MidiKeyboardState keyboardState;
    PolygonAudioProcessorEditor* editor{ nullptr };

    std::atomic<float>* gainParameter{ nullptr }, * velGammaParameter{ nullptr };
    std::atomic<float>* attackParameter{ nullptr }, * decayParameter{ nullptr }, * sustainParameter{ nullptr }, * releaseParameter{ nullptr };
    std::atomic<float>* orderParameter{ nullptr }, * teethParameter{ nullptr }, * foldParameter{ nullptr };
    std::atomic<float>* rotationParameter{ nullptr }, * fmRatioParameter{ nullptr }, * fmAmtParameter{ nullptr };

    int numVoices{ 10 };
    ADSR::Parameters envParams;
    Synthesiser synthesiser;
    static constexpr int samplesPerSubBlock = 32;

    static constexpr int numCoeffs{ 6 };
    hiir::Downsampler2xFpu<numCoeffs> downsamplerL, downsamplerR;
    static constexpr int numCoeffs2{ 6 };
    hiir::Downsampler2xFpu<numCoeffs2> downsampler2L, downsampler2R;
    static constexpr int numCoeffs3{ 6 };
    hiir::Downsampler2xFpu<numCoeffs3> downsampler3L, downsampler3R;
    static constexpr int numCoeffs4{ 6 };
    hiir::Downsampler2xFpu<numCoeffs4> downsampler4L, downsampler4R;

    DcBlocker dcBlockerL, dcBlockerR;

    static constexpr int maxOversampleFactor{ 16 };
    int oversampleFactor{ 2 };
    AudioBuffer<float> oversampledBuffer;

    std::shared_ptr<RingBuffer<float>> ringBuffer;

    //==============================================================================
    void updateEnvParams();
    void updateVoiceParams();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolygonAudioProcessor)
};
