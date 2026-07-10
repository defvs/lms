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

#include "TrackRating.hpp"

#include <array>
#include <optional>

#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>

#include "core/Service.hpp"
#include "database/objects/Types.hpp"
#include "database/objects/User.hpp"
#include "services/feedback/IFeedbackService.hpp"

#include "LmsApplication.hpp"

namespace lms::ui
{
    std::unique_ptr<Wt::WWidget> createTrackRating(db::TrackId trackId, std::string_view trackName)
    {
        constexpr std::size_t ratingCount{ 5 };

        auto container{ std::make_unique<Wt::WContainerWidget>() };
        container->setInline(true);
        container->setAttributeValue("role", "radiogroup");

        auto buttons{ std::make_shared<std::array<Wt::WPushButton*, ratingCount>>() };
        auto currentRating{ std::make_shared<std::optional<db::Rating>>(
            core::Service<feedback::IFeedbackService>::get()->getRating(LmsApp->getUserId(), trackId)) };

        for (std::size_t index{}; index < ratingCount; ++index)
        {
            const db::Rating rating{ static_cast<db::Rating>(index + 1) };
            Wt::WPushButton* button{ container->addNew<Wt::WPushButton>(Wt::WString{}, Wt::TextFormat::XHTML) };
            button->addStyleClass("btn btn-sm border-0 bg-transparent px-0");
            button->setAttributeValue("role", "radio");
            button->setAttributeValue("aria-label", Wt::WString::tr("Lms.Explore.rate-item").arg(std::string{ trackName }).arg(rating));
            (*buttons)[index] = button;
        }

        auto refresh{ [buttons, currentRating] {
            for (std::size_t index{}; index < ratingCount; ++index)
            {
                const db::Rating rating{ static_cast<db::Rating>(index + 1) };
                const bool selected{ *currentRating && rating <= **currentRating };
                Wt::WPushButton* button{ (*buttons)[index] };
                button->setText(Wt::WString::fromUTF8(selected ? "<i class=\"fa fa-fw fa-star\"></i>" : "<i class=\"fa fa-fw fa-star-o\"></i>"));
                button->toggleStyleClass("text-warning", selected);
                button->setAttributeValue("aria-checked", *currentRating && rating == **currentRating ? "true" : "false");
            }
        } };

        refresh();

        LmsApp->trackRatingChanged().connect(container.get(), [trackId, currentRating, refresh](db::TrackId changedTrackId, std::optional<db::Rating> rating) {
            if (changedTrackId != trackId)
                return;

            *currentRating = rating;
            refresh();
        });

        for (std::size_t index{}; index < ratingCount; ++index)
        {
            const db::Rating rating{ static_cast<db::Rating>(index + 1) };
            (*buttons)[index]->clicked().connect([trackId, rating, currentRating, refresh] {
                const std::optional<db::Rating> newRating{ *currentRating == rating ? std::nullopt : std::optional{ rating } };
                core::Service<feedback::IFeedbackService>::get()->setRating(LmsApp->getUserId(), trackId, newRating);
                LmsApp->trackRatingChanged().emit(trackId, newRating);
            });
        }

        return container;
    }
} // namespace lms::ui
