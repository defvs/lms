/*
 * Copyright (C) 2026 Emeric Poupon
 *
 * This file is part of LMS.
 *
 * LMS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include <memory>
#include <string_view>

#include <Wt/WWidget.h>

#include "database/objects/TrackId.hpp"

namespace lms::ui
{
    std::unique_ptr<Wt::WWidget> createTrackRating(db::TrackId trackId, std::string_view trackName);
}
