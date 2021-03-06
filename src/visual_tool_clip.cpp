// Copyright (c) 2011, Thomas Goyne <plorkyeran@aegisub.org>
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
// Aegisub Project http://www.aegisub.org/

/// @file visual_tool_clip.cpp
/// @brief Rectangular clipping visual typesetting tool
/// @ingroup visual_ts

#include "config.h"

#include "visual_tool_clip.h"

#include "ass_dialogue.h"
#include "include/aegisub/context.h"
#include "selection_controller.h"
#include "utils.h"

#include <boost/format.hpp>
#include <utility>

VisualToolClip::VisualToolClip(VideoDisplay *parent, agi::Context *context)
: VisualTool<ClipCorner>(parent, context)
, cur_1(0, 0)
, cur_2(video_res)
{
	ClipCorner *feats[4];
	for (auto& feat : feats) {
		feat = new ClipCorner;
		features.push_back(*feat);
	}

	// Attach each feature to the two features it shares edges with
	// Top-left
	int i = 0;
	feats[i]->horiz = feats[1];
	feats[i]->vert = feats[2];
	i++;

	// Top-right
	feats[i]->horiz = feats[0];
	feats[i]->vert = feats[3];
	i++;

	// Bottom-left
	feats[i]->horiz = feats[3];
	feats[i]->vert = feats[0];
	i++;

	// Bottom-right
	feats[i]->horiz = feats[2];
	feats[i]->vert = feats[1];
}

void VisualToolClip::Draw() {
	if (!active_line) return;

	DrawAllFeatures();

	// Draw rectangle
	gl.SetLineColour(colour[3], 1.0f, 2);
	gl.SetFillColour(colour[3], 0.0f);
	gl.DrawRectangle(cur_1, cur_2);

	// Draw outside area
	gl.SetLineColour(colour[3], 0.0f);
	gl.SetFillColour(*wxBLACK, 0.5f);
	if (inverse) {
		gl.DrawRectangle(cur_1, cur_2);
	}
	else {
		Vector2D v_min = video_pos;
		Vector2D v_max = video_pos + video_res;
		Vector2D c_min = cur_1.Min(cur_2);
		Vector2D c_max = cur_1.Max(cur_2);
		gl.DrawRectangle(v_min,                  Vector2D(v_max, c_min));
		gl.DrawRectangle(Vector2D(v_min, c_max), v_max);
		gl.DrawRectangle(Vector2D(v_min, c_min), Vector2D(c_min, c_max));
		gl.DrawRectangle(Vector2D(c_max, c_min), Vector2D(v_max, c_max));
	}
}

bool VisualToolClip::InitializeHold() {
	return true;
}

void VisualToolClip::UpdateHold() {
	// Limit to video area
	cur_1 = video_pos.Max((video_pos + video_res).Min(drag_start));
	cur_2 = video_pos.Max((video_pos + video_res).Min(mouse_pos));

	SetFeaturePositions();
	CommitHold();
}

void VisualToolClip::CommitHold() {
	std::string value = str(boost::format("(%s,%s)") % ToScriptCoords(cur_1.Min(cur_2)).Str() % ToScriptCoords(cur_1.Max(cur_2)).Str());

	for (auto line : c->selectionController->GetSelectedSet()) {
		// This check is technically not correct as it could be outside of an
		// override block... but that's rather unlikely
		bool has_iclip = line->Text.get().find("\\iclip") != std::string::npos;
		SetOverride(line, has_iclip ? "\\iclip" : "\\clip", value);
	}
}

void VisualToolClip::UpdateDrag(ClipCorner *feature) {
	// Update features which share an edge with the dragged one
	feature->horiz->pos = Vector2D(feature->horiz->pos, feature->pos);
	feature->vert->pos = Vector2D(feature->pos, feature->vert->pos);

	cur_1 = features.front().pos;
	cur_2 = features.back().pos;

	CommitHold();
}

void VisualToolClip::SetFeaturePositions() {
	auto it = features.begin();
	(it++)->pos = cur_1; // Top-left
	(it++)->pos = Vector2D(cur_2, cur_1); // Top-right
	(it++)->pos = Vector2D(cur_1, cur_2); // Bottom-left
	it->pos = cur_2; // Bottom-right
}

void VisualToolClip::DoRefresh() {
	if (active_line) {
		GetLineClip(active_line, cur_1, cur_2, inverse);
		cur_1 = FromScriptCoords(cur_1);
		cur_2 = FromScriptCoords(cur_2);
		SetFeaturePositions();
	}
}
