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
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    static void generateWavetable(size_t wavetableSize, std::vector<float>& waveX, std::vector<float>& waveY, const std::vector<Point<float>> vertices, float rotation, float teeth, float fold);

    //==============================================================================
    void setNumVoices(int newNumVoices);
    int getNumVoices();
    void setOversampleFactor(int newOversampleFactor);
    int getOversampleFactor() const { return oversampleFactor; }
    void setWavetableSize(int newWavetableSize);
    int getWavetableSize() const { return static_cast<int>(wavetable.size()); }
    std::vector<float>& getWavetableX() { return waveX; }
    std::vector<float>& getWavetableY() { return waveY; }

private:
    AudioProcessorValueTreeState parameters;
    MidiKeyboardState keyboardState;

    int numVoices{ 10 };
    ADSR::Parameters envParams;
    Synthesiser synthesiser;
    static constexpr int samplesPerSubBlock = 32;

    static constexpr int numCoeffs{ 6 };
    hiir::Downsampler2xFpu<numCoeffs> downsampler;
    static constexpr int numCoeffs2{ 6 };
    hiir::Downsampler2xFpu<numCoeffs> downsampler2;
    static constexpr int numCoeffs3{ 6 };
    hiir::Downsampler2xFpu<numCoeffs> downsampler3;
    static constexpr int numCoeffs4{ 6 };
    hiir::Downsampler2xFpu<numCoeffs> downsampler4;

    static constexpr int maxOversampleFactor{ 16 };
    int oversampleFactor{ 8 };
    AudioBuffer<float> oversampledBuffer;

    DcBlocker dcBlocker;

    int nextWavetableSize{ 2048 };
    std::vector<float> wavetable;

    std::vector<float> waveX, waveY;// = std::vector<float>(2048, 0), waveY = std::vector<float>(2048, 0);

    //==============================================================================
    void setPath(int numSamples);
    void updateEnvParams();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolygonAudioProcessor)
};
