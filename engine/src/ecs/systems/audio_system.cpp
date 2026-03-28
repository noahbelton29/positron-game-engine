//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 28/03/2026.
//

#define MINIAUDIO_IMPLEMENTATION
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#define MA_ENABLE_WASAPI
#define MA_ENABLE_COREAUDIO
#define MA_ENABLE_ALSA
#define MA_ENABLE_PULSEAUDIO
#include <miniaudio.h>

#include "positron/core/log.h"
#include "positron/ecs/component/components.h"
#include "positron/ecs/core/world.h"
#include "positron/ecs/systems/audio_system.h"

#include <algorithm>
#include <cstring>
#include <glm/glm.hpp>

namespace Positron {
    struct AudioSystem::Impl {
        ma_engine engine = {};
        bool      ready  = false;
    };

    namespace {
        float attenuate(const float dist, const float rolloff, const float maxDist) {
            if (maxDist <= 0.f)
                return 1.f;
            if (dist <= rolloff)
                return 1.f;
            if (dist >= maxDist)
                return 0.f;
            return 1.f - (dist - rolloff) / (maxDist - rolloff);
        }

        ma_sound *toSound(void *p) { return static_cast<ma_sound *>(p); }

        ma_sound *loadSound(ma_engine &engine, const std::string &path, const bool loop) {
            auto *sound = new ma_sound;
            std::memset(sound, 0, sizeof(ma_sound));

            if (constexpr ma_uint32 flags = MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC;
                ma_sound_init_from_file(&engine, path.c_str(), flags, nullptr, nullptr, sound) != MA_SUCCESS) {
                POSITRON_ERROR("AudioSystem: failed to load '{}'", path);
                delete sound;
                return nullptr;
            }

            ma_sound_set_looping(sound, loop ? MA_TRUE : MA_FALSE);
            return sound;
        }

        void destroySound(void *&p) {
            if (!p)
                return;
            ma_sound_uninit(toSound(p));
            delete toSound(p);
            p = nullptr;
        }
    } // namespace

    AudioSystem::AudioSystem() : pImpl_(std::make_unique<Impl>()) {}
    AudioSystem::~AudioSystem() = default;

    void AudioSystem::onInit(World &) {
        const ma_engine_config cfg = ma_engine_config_init();
        if (ma_engine_init(&cfg, &pImpl_->engine) != MA_SUCCESS) {
            POSITRON_ERROR("AudioSystem: failed to initialise miniaudio engine");
            return;
        }
        pImpl_->ready = true;
        POSITRON_INFO("AudioSystem initialised");
    }

    void AudioSystem::onUpdate(World &world) {
        if (!pImpl_->ready)
            return;

        glm::vec3 listenerPos = {0.f, 0.f, 0.f};
        world.each<AudioListenerComponent, TransformComponent>(
                [&](Entity, const AudioListenerComponent &, const TransformComponent &tf) {
                    listenerPos = tf.position;
                });

        world.each<AudioSourceComponent, TransformComponent>(
                [&](Entity, AudioSourceComponent &src, const TransformComponent &tf) {
                    if (!src._loaded) {
                        if (src.path.empty())
                            return;
                        src._sound = loadSound(pImpl_->engine, src.path, src.loop);
                        if (!src._sound)
                            return;
                        src._loaded = true;

                        ma_sound_set_pitch(toSound(src._sound), src.pitch);

                        if (src.playOnAwake) {
                            ma_sound_start(toSound(src._sound));
                            src._playing = true;
                        }
                    }

                    if (!src._sound)
                        return;

                    const float dist  = glm::distance(tf.position, listenerPos);
                    const float atten = attenuate(dist, src.rolloffDistance, src.maxDistance);
                    ma_sound_set_volume(toSound(src._sound), src.volume * atten);

                    ma_sound_set_looping(toSound(src._sound), src.loop ? MA_TRUE : MA_FALSE);
                });
    }

    void AudioSystem::onShutdown(World &world) {
        if (!pImpl_->ready)
            return;

        world.each<AudioSourceComponent>([&](Entity, AudioSourceComponent &src) {
            if (src._sound) {
                ma_sound_stop(toSound(src._sound));
                destroySound(src._sound);
                src._loaded  = false;
                src._playing = false;
            }
        });

        ma_engine_uninit(&pImpl_->engine);
        pImpl_->ready = false;
    }

    void AudioSystem::play(World &world, const Entity entity) const {
        if (!pImpl_->ready || !world.hasComponent<AudioSourceComponent>(entity))
            return;
        auto &src = world.getComponent<AudioSourceComponent>(entity);
        if (!src._sound)
            return;

        if (ma_sound_is_playing(toSound(src._sound)))
            return;
        ma_sound_seek_to_pcm_frame(toSound(src._sound), 0);
        ma_sound_start(toSound(src._sound));
        src._playing = true;
    }

    void AudioSystem::stop(World &world, const Entity entity) const {
        if (!pImpl_->ready || !world.hasComponent<AudioSourceComponent>(entity))
            return;
        auto &src = world.getComponent<AudioSourceComponent>(entity);
        if (!src._sound)
            return;
        ma_sound_stop(toSound(src._sound));
        src._playing = false;
    }

} // namespace Positron
