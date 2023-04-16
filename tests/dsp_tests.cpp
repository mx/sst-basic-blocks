//
// Created by Paul Walker on 4/3/23.
//

#include "catch2.hpp"
#include "smoke_test_sse.h"
#include <cmath>
#include "sst/basic-blocks/dsp/BlockInterpolators.h"
#include "sst/basic-blocks/dsp/QuadratureOscillators.h"
#include "sst/basic-blocks/dsp/LanczosResampler.h"

#include <iostream>


TEST_CASE("lipol_sse basic", "[dsp]")
{
    SECTION("Block Size 16")
    {
        constexpr int bs{16};
        auto lip = sst::basic_blocks::dsp::lipol_sse<bs, false>();

        float where alignas(16)[bs];
        lip.set_target_instant(0.2);
        auto prev = 0.2;
        for (const auto &t : {0.6, 0.2, 0.4})
        {
            lip.set_target(t);
            lip.store_block(where);
            for (int i = 0; i < bs; i++)
            {
                REQUIRE(where[i] == Approx(prev + (t - prev) / bs * i).margin(1e-5));
            }
            prev = t;
        }
    }

    SECTION("Block Size with FirstRun")
    {
        constexpr int bs{16};
        auto lip = sst::basic_blocks::dsp::lipol_sse<bs, true>();

        float where alignas(16)[bs];
        auto prev = 0.2;
        for (const auto &t : {0.2, 0.6, 0.2, 0.4})
        {
            lip.set_target(t);
            lip.store_block(where);
            for (int i = 0; i < bs; i++)
            {
                REQUIRE(where[i] == Approx(prev + (t - prev) / bs * i).margin(1e-5));
            }
            prev = t;
        }
    }

    SECTION("Block Size without FirstRun starts at 0")
    {
        constexpr int bs{16};
        auto lip = sst::basic_blocks::dsp::lipol_sse<bs, false>();

        float where alignas(16)[bs];
        auto prev = 0.0;
        for (const auto &t : {0.2, 0.6, 0.2, 0.4})
        {
            lip.set_target(t);
            lip.store_block(where);
            for (int i = 0; i < bs; i++)
            {
                REQUIRE(where[i] == Approx(prev + (t - prev) / bs * i).margin(1e-5));
            }
            prev = t;
        }
    }

    SECTION("Block Size 32")
    {
        constexpr int bs{32};
        auto lip = sst::basic_blocks::dsp::lipol_sse<bs, false>();

        float where alignas(16)[bs];
        lip.set_target_instant(0.2);
        auto prev = 0.2;
        for (const auto &t : {0.6, 0.2, 0.4})
        {
            lip.set_target(t);
            lip.store_block(where);
            for (int i = 0; i < bs; i++)
            {
                REQUIRE(where[i] == Approx(prev + (t - prev) / bs * i).margin(1e-5));
            }
            prev = t;
        }
    }

    SECTION("Block Size 8")
    {
        constexpr int bs{8};
        auto lip = sst::basic_blocks::dsp::lipol_sse<bs, false>();

        float where alignas(16)[bs];
        lip.set_target_instant(0.2);
        auto prev = 0.2;
        for (const auto &t : {0.6, 0.2, 0.4})
        {
            lip.set_target(t);
            lip.store_block(where);
            for (int i = 0; i < bs; i++)
            {
                REQUIRE(where[i] == Approx(prev + (t - prev) / bs * i).margin(1e-5));
            }
            prev = t;
        }
    }
}

TEST_CASE("lipol_sse multiply_block", "[dsp]")
{
    SECTION("Block Size 16")
    {
        constexpr int bs{16};
        auto lip = sst::basic_blocks::dsp::lipol_sse<bs, false>();

        lip.set_target_instant(0.2);
        lip.set_target(0.6);
        float f alignas(16)[bs], r alignas(16)[bs];
        for (int i = 0; i < bs; ++i)
        {
            f[i] = std::sin(i * 0.2 * M_PI);
        }
        lip.multiply_block_to(f, r);
        for (int i = 0; i < bs; i++)
        {
            auto x = (0.2 + (0.6 - 0.2) / bs * i) * f[i];
            REQUIRE(x == Approx(r[i]).margin(1e-5));
        }
    }
}

TEST_CASE("lipol_sse fade_block", "[dsp]")
{
    SECTION("Block Size 16")
    {
        constexpr int bs{16};
        auto lip = sst::basic_blocks::dsp::lipol_sse<bs, false>();

        lip.set_target_instant(0.2);
        lip.set_target(0.6);
        float f alignas(16)[bs], g alignas(16)[bs], r alignas(16)[bs];
        for (int i = 0; i < bs; ++i)
        {
            f[i] = std::sin(i * 0.2 * M_PI);
            g[i] = std::cos(i * 0.3 * M_PI);
        }
        lip.fade_blocks(f, g, r);
        for (int i = 0; i < bs; i++)
        {
            auto cx = (0.2 + (0.6 - 0.2) / bs * i);
            auto rx = f[i] * (1 - cx) + g[i] * cx;
            REQUIRE(rx == Approx(r[i]).margin(1e-5));
        }
    }
}

TEST_CASE("Quadrature Oscillator")
{
    for (const auto omega : {0.04, 0.12, 0.43, 0.97})
    {
        DYNAMIC_SECTION("Vicanek Quadrature omega=" << omega)
        {
            auto q = sst::basic_blocks::dsp::QuadratureOscillator();

            float p0 = 0;
            q.setRate(omega);
            for (int i = 0; i < 200; ++i)
            {
                REQUIRE(q.v == Approx(sin(p0)).margin(1e-3));
                REQUIRE(q.u == Approx(cos(p0)).margin(1e-3));

                q.step();
                p0 += omega;
            }
        }
    }
}

TEST_CASE("LanczosResampler", "[dsp]")
{
    SECTION("Can Interpolate Sine")
    {
        sst::basic_blocks::dsp::LanczosResampler<32> lr(48000, 88100);

        // plot 'lancos_raw.csv' using 1:2 with lines, 'lancos_samp.csv' using 1:2 with lines
        int points = 1000;
        double dp = 1.0 / 370;
        float phase = 0;
        for (auto i = 0; i < points; ++i)
        {
            auto obsS = std::sin(phase * 2.0 * M_PI);
            auto obsR = phase * 2 - 1;
            phase += dp;
            if (phase > 1)
                phase -= 1;
            lr.push(obsS, obsR);
        }

        float outBlock alignas(16) [64], outBlockR alignas(16)[64];
        int q, gen;
        dp /= 88100.0 / 48000.0;

        phase = 0;
        while ((gen = lr.populateNext(outBlock, outBlockR, 64)) > 0)
        {
            for (int i = 0; i < gen; ++i)
            {
                auto d = outBlock[i] - std::sin(phase * 2.0 * M_PI);
                REQUIRE(fabs(d) < 0.025);
                phase += dp;
            }
        }
    }
}