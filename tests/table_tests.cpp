/*
 * sst-basic-blocks - an open source library of core audio utilities
 * built by Surge Synth Team.
 *
 * Provides a collection of tools useful on the audio thread for blocks,
 * modulation, etc... or useful for adapting code to multiple environments.
 *
 * Copyright 2023, various authors, as described in the GitHub
 * transaction log. Parts of this code are derived from similar
 * functions original in Surge or ShortCircuit.
 *
 * sst-basic-blocks is released under the GNU General Public Licence v3
 * or later (GPL-3.0-or-later). The license is found in the "LICENSE"
 * file in the root of this repository, or at
 * https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * All source in sst-basic-blocks available at
 * https://github.com/surge-synthesizer/sst-basic-blocks
 */

#include "catch2.hpp"
#include "smoke_test_sse.h"

#include "sst/basic-blocks/tables/DbToLinearProvider.h"

namespace tabl = sst::basic_blocks::tables;

TEST_CASE("DB to Linear", "[tables]")
{
    tabl::DbToLinearProvider dbt;
    dbt.init();

    for (float db = -192; db < 10; db += 0.0327)
    {
        INFO("Testing at " << db);
        REQUIRE(dbt.dbToLinear(db) == Approx(pow(10, db / 20.0)).margin(0.01));
    }
}