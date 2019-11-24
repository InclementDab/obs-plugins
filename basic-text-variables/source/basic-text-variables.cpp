#include "basic-text-variables.h"

void get_defaults(obs_data_t* settings)
{
	obs_data_t* font_obj = obs_data_create();
	obs_data_set_default_string(font_obj, "face", "Arial");
	obs_data_set_default_int(font_obj, "size", 36);

	obs_data_set_default_obj(settings, S_FONT, font_obj);
	obs_data_set_default_string(settings, S_ALIGN, S_ALIGN_LEFT);
	obs_data_set_default_string(settings, S_VALIGN, S_VALIGN_TOP);
	obs_data_set_default_int(settings, S_COLOR, 0xFFFFFF);
	obs_data_set_default_int(settings, S_OPACITY, 100);
	obs_data_set_default_int(settings, S_GRADIENT_COLOR, 0xFFFFFF);
	obs_data_set_default_int(settings, S_GRADIENT_OPACITY, 100);
	obs_data_set_default_double(settings, S_GRADIENT_DIR, 90.0);
	obs_data_set_default_int(settings, S_BKCOLOR, 0x000000);
	obs_data_set_default_int(settings, S_BKOPACITY, 0);
	obs_data_set_default_int(settings, S_OUTLINE_SIZE, 2);
	obs_data_set_default_int(settings, S_OUTLINE_COLOR, 0xFFFFFF);
	obs_data_set_default_int(settings, S_OUTLINE_OPACITY, 100);
	obs_data_set_default_int(settings, S_CHATLOG_LINES, 6);
	obs_data_set_default_bool(settings, S_EXTENTS_WRAP, true);
	obs_data_set_default_int(settings, S_EXTENTS_CX, 100);
	obs_data_set_default_int(settings, S_EXTENTS_CY, 100);
	obs_data_set_default_int(settings, S_TRANSFORM,
							 S_TRANSFORM_NONE);

	obs_data_release(font_obj);
}

static obs_properties_t* get_properties(void* data)
{
	TextSource* s = reinterpret_cast<TextSource*>(data);
	string path;

	obs_properties_t* props = obs_properties_create();
	obs_property_t* p;

	obs_properties_add_font(props, S_FONT, T_FONT);

	p = obs_properties_add_bool(props, S_USE_FILE, T_USE_FILE);
	obs_property_set_modified_callback(p, use_file_changed);

	string filter;
	filter += T_FILTER_TEXT_FILES;
	filter += " (*.txt);;";
	filter += T_FILTER_ALL_FILES;
	filter += " (*.*)";

	if (s && !s->file.empty()) {
		const char* slash;

		path = s->file;
		replace(path.begin(), path.end(), '\\', '/');
		slash = strrchr(path.c_str(), '/');
		if (slash)
			path.resize(slash - path.c_str() + 1);
	}

	obs_properties_add_text(props, S_TEXT, T_TEXT, OBS_TEXT_MULTILINE);
	obs_properties_add_button(props, S_VARIABLES, T_VARIABLES, hint_clicked);
	obs_properties_add_path(props, S_FILE, T_FILE, OBS_PATH_FILE,
							filter.c_str(), path.c_str());

	p = obs_properties_add_list(props, S_TRANSFORM, T_TRANSFORM,
								OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);
	obs_property_list_add_int(p, T_TRANSFORM_NONE, S_TRANSFORM_NONE);
	obs_property_list_add_int(p, T_TRANSFORM_UPPERCASE,
							  S_TRANSFORM_UPPERCASE);
	obs_property_list_add_int(p, T_TRANSFORM_LOWERCASE,
							  S_TRANSFORM_LOWERCASE);
	obs_property_list_add_int(p, T_TRANSFORM_STARTCASE,
							  S_TRANSFORM_STARTCASE);

	obs_properties_add_bool(props, S_VERTICAL, T_VERTICAL);
	obs_properties_add_color(props, S_COLOR, T_COLOR);
	p = obs_properties_add_int_slider(props, S_OPACITY, T_OPACITY, 0, 100,
									  1);
	obs_property_int_set_suffix(p, "%");

	p = obs_properties_add_bool(props, S_GRADIENT, T_GRADIENT);
	obs_property_set_modified_callback(p, gradient_changed);

	obs_properties_add_color(props, S_GRADIENT_COLOR, T_GRADIENT_COLOR);
	p = obs_properties_add_int_slider(props, S_GRADIENT_OPACITY,
									  T_GRADIENT_OPACITY, 0, 100, 1);
	obs_property_int_set_suffix(p, "%");
	obs_properties_add_float_slider(props, S_GRADIENT_DIR, T_GRADIENT_DIR,
									0, 360, 0.1);

	obs_properties_add_color(props, S_BKCOLOR, T_BKCOLOR);
	p = obs_properties_add_int_slider(props, S_BKOPACITY, T_BKOPACITY, 0,
									  100, 1);
	obs_property_int_set_suffix(p, "%");

	p = obs_properties_add_list(props, S_ALIGN, T_ALIGN,
								OBS_COMBO_TYPE_LIST,
								OBS_COMBO_FORMAT_STRING);
	obs_property_list_add_string(p, T_ALIGN_LEFT, S_ALIGN_LEFT);
	obs_property_list_add_string(p, T_ALIGN_CENTER, S_ALIGN_CENTER);
	obs_property_list_add_string(p, T_ALIGN_RIGHT, S_ALIGN_RIGHT);

	p = obs_properties_add_list(props, S_VALIGN, T_VALIGN,
								OBS_COMBO_TYPE_LIST,
								OBS_COMBO_FORMAT_STRING);
	obs_property_list_add_string(p, T_VALIGN_TOP, S_VALIGN_TOP);
	obs_property_list_add_string(p, T_VALIGN_CENTER, S_VALIGN_CENTER);
	obs_property_list_add_string(p, T_VALIGN_BOTTOM, S_VALIGN_BOTTOM);

	p = obs_properties_add_bool(props, S_OUTLINE, T_OUTLINE);
	obs_property_set_modified_callback(p, outline_changed);

	obs_properties_add_int(props, S_OUTLINE_SIZE, T_OUTLINE_SIZE, 1, 20, 1);
	obs_properties_add_color(props, S_OUTLINE_COLOR, T_OUTLINE_COLOR);
	p = obs_properties_add_int_slider(props, S_OUTLINE_OPACITY,
									  T_OUTLINE_OPACITY, 0, 100, 1);
	obs_property_int_set_suffix(p, "%");

	p = obs_properties_add_bool(props, S_CHATLOG_MODE, T_CHATLOG_MODE);
	obs_property_set_modified_callback(p, chatlog_mode_changed);

	obs_properties_add_int(props, S_CHATLOG_LINES, T_CHATLOG_LINES, 1, 1000,
						   1);

	p = obs_properties_add_bool(props, S_EXTENTS, T_EXTENTS);
	obs_property_set_modified_callback(p, extents_modified);

	obs_properties_add_int(props, S_EXTENTS_CX, T_EXTENTS_CX, 32, 8000, 1);
	obs_properties_add_int(props, S_EXTENTS_CY, T_EXTENTS_CY, 32, 8000, 1);
	obs_properties_add_bool(props, S_EXTENTS_WRAP, T_EXTENTS_WRAP);

	return props;
}

bool hint_clicked(obs_properties_t* props, obs_property_t* property, void* data)
{
	blog(LOG_DEBUG, "hint_clicked");

	const gs_init_data gs_data;

	obs_display_t* display = obs_display_create(&gs_data, 0);



	return true;
}


#pragma region TextSource

void TextSource::UpdateFont()
{
	hfont = nullptr;
	font.reset(nullptr);

	LOGFONT lf = {};
	lf.lfHeight = face_size;
	lf.lfWeight = bold ? FW_BOLD : FW_DONTCARE;
	lf.lfItalic = italic;
	lf.lfUnderline = underline;
	lf.lfStrikeOut = strikeout;
	lf.lfQuality = ANTIALIASED_QUALITY;
	lf.lfCharSet = DEFAULT_CHARSET;

	if (!face.empty()) {
		wcscpy_s(lf.lfFaceName, face.c_str());
		hfont = CreateFontIndirect(&lf);
	}

	if (!hfont) {
		wcscpy_s(lf.lfFaceName, L"Arial");
		hfont = CreateFontIndirect(&lf);
	}

	if (hfont)
		font.reset(new Font(hdc, hfont));
}

void TextSource::GetStringFormat(StringFormat& format)
{
	UINT flags = StringFormatFlagsNoFitBlackBox |
		StringFormatFlagsMeasureTrailingSpaces;

	if (vertical)
		flags |= StringFormatFlagsDirectionVertical |
		StringFormatFlagsDirectionRightToLeft;

	format.SetFormatFlags(flags);
	format.SetTrimming(StringTrimmingWord);

	switch (align) {
		case Align::Left:
			if (vertical)
				format.SetLineAlignment(StringAlignmentFar);
			else
				format.SetAlignment(StringAlignmentNear);
			break;
		case Align::Center:
			if (vertical)
				format.SetLineAlignment(StringAlignmentCenter);
			else
				format.SetAlignment(StringAlignmentCenter);
			break;
		case Align::Right:
			if (vertical)
				format.SetLineAlignment(StringAlignmentNear);
			else
				format.SetAlignment(StringAlignmentFar);
	}

	switch (valign) {
		case VAlign::Top:
			if (vertical)
				format.SetAlignment(StringAlignmentNear);
			else
				format.SetLineAlignment(StringAlignmentNear);
			break;
		case VAlign::Center:
			if (vertical)
				format.SetAlignment(StringAlignmentCenter);
			else
				format.SetLineAlignment(StringAlignmentCenter);
			break;
		case VAlign::Bottom:
			if (vertical)
				format.SetAlignment(StringAlignmentFar);
			else
				format.SetLineAlignment(StringAlignmentFar);
	}
}

/* GDI+ treats '\n' as an extra character with an actual render size when
 * calculating the texture size, so we have to calculate the size of '\n' to
 * remove the padding.  Because we always add a newline to the string, we
 * also remove the extra unused newline. */
void TextSource::RemoveNewlinePadding(const StringFormat& format, RectF& box)
{
	RectF before;
	RectF after;
	Status stat;

	stat = graphics.MeasureString(L"W", 2, font.get(), PointF(0.0f, 0.0f),
								  &format, &before);
	warn_stat("MeasureString (without newline)");

	stat = graphics.MeasureString(L"W\n", 3, font.get(), PointF(0.0f, 0.0f),
								  &format, &after);
	warn_stat("MeasureString (with newline)");

	float offset_cx = after.Width - before.Width;
	float offset_cy = after.Height - before.Height;

	if (!vertical) {
		if (offset_cx >= 1.0f)
			offset_cx -= 1.0f;

		if (valign == VAlign::Center)
			box.Y -= offset_cy * 0.5f;
		else if (valign == VAlign::Bottom)
			box.Y -= offset_cy;
	}
	else {
		if (offset_cy >= 1.0f)
			offset_cy -= 1.0f;

		if (align == Align::Center)
			box.X -= offset_cx * 0.5f;
		else if (align == Align::Right)
			box.X -= offset_cx;
	}

	box.Width -= offset_cx;
	box.Height -= offset_cy;
}

void TextSource::CalculateTextSizes(const StringFormat& format,
									RectF& bounding_box, SIZE& text_size)
{
	RectF layout_box;
	RectF temp_box;
	Status stat;

	if (!text.empty()) {
		if (use_extents && wrap) {
			layout_box.X = layout_box.Y = 0;
			layout_box.Width = float(extents_cx);
			layout_box.Height = float(extents_cy);

			if (use_outline) {
				layout_box.Width -= outline_size;
				layout_box.Height -= outline_size;
			}

			stat = graphics.MeasureString(text.c_str(),
				(int)text.size() + 1,
										  font.get(), layout_box,
										  &format, &bounding_box);
			warn_stat("MeasureString (wrapped)");

			temp_box = bounding_box;
		}
		else {
			stat = graphics.MeasureString(
				text.c_str(), (int)text.size() + 1, font.get(),
				PointF(0.0f, 0.0f), &format, &bounding_box);
			warn_stat("MeasureString (non-wrapped)");

			temp_box = bounding_box;

			bounding_box.X = 0.0f;
			bounding_box.Y = 0.0f;

			RemoveNewlinePadding(format, bounding_box);

			if (use_outline) {
				bounding_box.Width += outline_size;
				bounding_box.Height += outline_size;
			}
		}
	}

	if (vertical) {
		if (bounding_box.Width < face_size) {
			text_size.cx = face_size;
			bounding_box.Width = float(face_size);
		}
		else {
			text_size.cx = LONG(bounding_box.Width + EPSILON);
		}

		text_size.cy = LONG(bounding_box.Height + EPSILON);
	}
	else {
		if (bounding_box.Height < face_size) {
			text_size.cy = face_size;
			bounding_box.Height = float(face_size);
		}
		else {
			text_size.cy = LONG(bounding_box.Height + EPSILON);
		}

		text_size.cx = LONG(bounding_box.Width + EPSILON);
	}

	if (use_extents) {
		text_size.cx = extents_cx;
		text_size.cy = extents_cy;
	}

	text_size.cx += text_size.cx % 2;
	text_size.cy += text_size.cy % 2;

	int64_t total_size = int64_t(text_size.cx) * int64_t(text_size.cy);

	/* GPUs typically have texture size limitations */
	clamp(text_size.cx, MIN_SIZE_CX, MAX_SIZE_CX);
	clamp(text_size.cy, MIN_SIZE_CY, MAX_SIZE_CY);

	/* avoid taking up too much VRAM */
	if (total_size > MAX_AREA) {
		if (text_size.cx > text_size.cy)
			text_size.cx = (LONG)MAX_AREA / text_size.cy;
		else
			text_size.cy = (LONG)MAX_AREA / text_size.cx;
	}

	/* the internal text-rendering bounding box for is reset to
	 * its internal value in case the texture gets cut off */
	bounding_box.Width = temp_box.Width;
	bounding_box.Height = temp_box.Height;
}

void TextSource::RenderOutlineText(Graphics& graphics, const GraphicsPath& path,
								   const Brush& brush)
{
	DWORD outline_rgba = calc_color(outline_color, outline_opacity);
	Status stat;

	Pen pen(Color(outline_rgba), outline_size);
	stat = pen.SetLineJoin(LineJoinRound);
	warn_stat("pen.SetLineJoin");

	stat = graphics.DrawPath(&pen, &path);
	warn_stat("graphics.DrawPath");

	stat = graphics.FillPath(&brush, &path);
	warn_stat("graphics.FillPath");
}

void TextSource::RenderText()
{
	StringFormat format(StringFormat::GenericTypographic());
	Status stat;

	RectF box;
	SIZE size;

	GetStringFormat(format);
	CalculateTextSizes(format, box, size);

	unique_ptr<uint8_t[]> bits(new uint8_t[size.cx * size.cy * 4]);
	Bitmap bitmap(size.cx, size.cy, 4 * size.cx, PixelFormat32bppARGB,
				  bits.get());

	Graphics graphics_bitmap(&bitmap);
	LinearGradientBrush brush(RectF(0, 0, (float)size.cx, (float)size.cy),
							  Color(calc_color(color, opacity)),
							  Color(calc_color(color2, opacity2)),
							  gradient_dir, 1);
	DWORD full_bk_color = bk_color & 0xFFFFFF;

	if (!text.empty() || use_extents)
		full_bk_color |= get_alpha_val(bk_opacity);

	if ((size.cx > box.Width || size.cy > box.Height) && !use_extents) {
		stat = graphics_bitmap.Clear(Color(0));
		warn_stat("graphics_bitmap.Clear");

		SolidBrush bk_brush = Color(full_bk_color);
		stat = graphics_bitmap.FillRectangle(&bk_brush, box);
		warn_stat("graphics_bitmap.FillRectangle");
	}
	else {
		stat = graphics_bitmap.Clear(Color(full_bk_color));
		warn_stat("graphics_bitmap.Clear");
	}

	graphics_bitmap.SetTextRenderingHint(TextRenderingHintAntiAlias);
	graphics_bitmap.SetCompositingMode(CompositingModeSourceOver);
	graphics_bitmap.SetSmoothingMode(SmoothingModeAntiAlias);

	if (!text.empty()) {
		if (use_outline) {
			box.Offset(outline_size / 2, outline_size / 2);

			FontFamily family;
			GraphicsPath path;

			font->GetFamily(&family);
			stat = path.AddString(text.c_str(), (int)text.size(),
								  &family, font->GetStyle(),
								  font->GetSize(), box, &format);
			warn_stat("path.AddString");

			RenderOutlineText(graphics_bitmap, path, brush);
		}
		else {
			stat = graphics_bitmap.DrawString(text.c_str(),
				(int)text.size(),
											  font.get(), box,
											  &format, &brush);
			warn_stat("graphics_bitmap.DrawString");
		}
	}

	if (!tex || (LONG)cx != size.cx || (LONG)cy != size.cy) {
		obs_enter_graphics();
		if (tex)
			gs_texture_destroy(tex);

		const uint8_t* data = (uint8_t*)bits.get();
		tex = gs_texture_create(size.cx, size.cy, GS_BGRA, 1, &data,
								GS_DYNAMIC);

		obs_leave_graphics();

		cx = (uint32_t)size.cx;
		cy = (uint32_t)size.cy;

	}
	else if (tex) {
		obs_enter_graphics();
		gs_texture_set_image(tex, bits.get(), size.cx * 4, false);
		obs_leave_graphics();
	}
}

const char* TextSource::GetMainString(const char* str)
{
	if (!str) return "";
	if (!chatlog_mode || !chatlog_lines) return str;

	int lines = chatlog_lines;

	size_t len = strlen(str);
	if (!len) return str;

	const char* temp = str + len;

	while (temp != str) {
		temp--;

		if (temp[0] == '\n' && temp[1] != 0) {
			if (!--lines)
				break;
		}
	}

	return *temp == '\n' ? temp + 1 : temp;

}

void TextSource::ReplaceVariables()
{
	for (text_method m : text_method_list) {

		text = std::regex_replace(text, m.get_regex(), m.func(this));
	}
}

void TextSource::LoadFileText()
{
	BPtr<char> file_text = os_quick_read_utf8_file(file.c_str());
	text = to_wide(GetMainString(file_text));

	if (!text.empty() && text.back() != '\n')
		text.push_back('\n');
}

void TextSource::TransformText()
{
	const locale loc = locale(obs_get_locale());
	const ctype<wchar_t>& f = use_facet<ctype<wchar_t>>(loc);
	if (text_transform == S_TRANSFORM_UPPERCASE)
		f.toupper(&text[0], &text[0] + text.size());
	else if (text_transform == S_TRANSFORM_LOWERCASE)
		f.tolower(&text[0], &text[0] + text.size());
	else if (text_transform == S_TRANSFORM_STARTCASE) {
		bool upper = true;
		for (wstring::iterator it = text.begin(); it != text.end();
			 ++it) {
			const wchar_t upper_char = f.toupper(*it);
			const wchar_t lower_char = f.tolower(*it);
			if (upper && lower_char != upper_char) {
				upper = false;
				*it = upper_char;
			}
			else if (lower_char != upper_char) {
				*it = lower_char;
			}
			else {
				upper = iswspace(*it);
			}
		}
	}
}

void TextSource::Update(obs_data_t* s)
{
	const char* new_text = obs_data_get_string(s, S_TEXT);
	obs_data_t* font_obj = obs_data_get_obj(s, S_FONT);
	const char* align_str = obs_data_get_string(s, S_ALIGN);
	const char* valign_str = obs_data_get_string(s, S_VALIGN);
	uint32_t new_color = obs_data_get_uint32(s, S_COLOR);
	uint32_t new_opacity = obs_data_get_uint32(s, S_OPACITY);
	bool gradient = obs_data_get_bool(s, S_GRADIENT);
	uint32_t new_color2 = obs_data_get_uint32(s, S_GRADIENT_COLOR);
	uint32_t new_opacity2 = obs_data_get_uint32(s, S_GRADIENT_OPACITY);
	float new_grad_dir = (float)obs_data_get_double(s, S_GRADIENT_DIR);
	bool new_vertical = obs_data_get_bool(s, S_VERTICAL);
	bool new_outline = obs_data_get_bool(s, S_OUTLINE);
	uint32_t new_o_color = obs_data_get_uint32(s, S_OUTLINE_COLOR);
	uint32_t new_o_opacity = obs_data_get_uint32(s, S_OUTLINE_OPACITY);
	uint32_t new_o_size = obs_data_get_uint32(s, S_OUTLINE_SIZE);
	bool new_use_file = obs_data_get_bool(s, S_USE_FILE);
	const char* new_file = obs_data_get_string(s, S_FILE);
	bool new_chat_mode = obs_data_get_bool(s, S_CHATLOG_MODE);
	int new_chat_lines = (int)obs_data_get_int(s, S_CHATLOG_LINES);
	bool new_extents = obs_data_get_bool(s, S_EXTENTS);
	bool new_extents_wrap = obs_data_get_bool(s, S_EXTENTS_WRAP);
	uint32_t n_extents_cx = obs_data_get_uint32(s, S_EXTENTS_CX);
	uint32_t n_extents_cy = obs_data_get_uint32(s, S_EXTENTS_CY);
	int new_text_transform = (int)obs_data_get_int(s, S_TRANSFORM);

	const char* font_face = obs_data_get_string(font_obj, "face");
	int font_size = (int)obs_data_get_int(font_obj, "size");
	int64_t font_flags = obs_data_get_int(font_obj, "flags");
	bool new_bold = (font_flags & OBS_FONT_BOLD) != 0;
	bool new_italic = (font_flags & OBS_FONT_ITALIC) != 0;
	bool new_underline = (font_flags & OBS_FONT_UNDERLINE) != 0;
	bool new_strikeout = (font_flags & OBS_FONT_STRIKEOUT) != 0;

	uint32_t new_bk_color = obs_data_get_uint32(s, S_BKCOLOR);
	uint32_t new_bk_opacity = obs_data_get_uint32(s, S_BKOPACITY);

	/* ----------------------------- */

	wstring new_face = to_wide(font_face);

	if (new_face != face || face_size != font_size || new_bold != bold ||
		new_italic != italic || new_underline != underline ||
		new_strikeout != strikeout) {

		face = new_face;
		face_size = font_size;
		bold = new_bold;
		italic = new_italic;
		underline = new_underline;
		strikeout = new_strikeout;

		UpdateFont();
	}

	/* ----------------------------- */

	new_color = rgb_to_bgr(new_color);
	new_color2 = rgb_to_bgr(new_color2);
	new_o_color = rgb_to_bgr(new_o_color);
	new_bk_color = rgb_to_bgr(new_bk_color);

	color = new_color;
	opacity = new_opacity;
	color2 = new_color2;
	opacity2 = new_opacity2;
	gradient_dir = new_grad_dir;
	vertical = new_vertical;

	bk_color = new_bk_color;
	bk_opacity = new_bk_opacity;
	use_extents = new_extents;
	wrap = new_extents_wrap;
	extents_cx = n_extents_cx;
	extents_cy = n_extents_cy;
	text_transform = new_text_transform;

	if (!gradient) {
		color2 = color;
		opacity2 = opacity;
	}

	read_from_file = new_use_file;

	chatlog_mode = new_chat_mode;
	chatlog_lines = new_chat_lines;

	if (read_from_file) {
		file = new_file;
		file_timestamp = get_modified_timestamp(new_file);
		LoadFileText();

	}
	else {
		text = to_wide(GetMainString(new_text));

		/* all text should end with newlines due to the fact that GDI+
		 * treats strings without newlines differently in terms of
		 * render size */
		if (!text.empty())
			text.push_back('\n');

	}
	ReplaceVariables();
	TransformText();
	

	use_outline = new_outline;
	outline_color = new_o_color;
	outline_opacity = new_o_opacity;
	outline_size = roundf(float(new_o_size));

	if (strcmp(align_str, S_ALIGN_CENTER) == 0)
		align = Align::Center;
	else if (strcmp(align_str, S_ALIGN_RIGHT) == 0)
		align = Align::Right;
	else
		align = Align::Left;

	if (strcmp(valign_str, S_VALIGN_CENTER) == 0)
		valign = VAlign::Center;
	else if (strcmp(valign_str, S_VALIGN_BOTTOM) == 0)
		valign = VAlign::Bottom;
	else
		valign = VAlign::Top;

	RenderText();
	update_time_elapsed = 0.0f;

	/* ----------------------------- */

	obs_data_release(font_obj);
}

void TextSource::Tick(float seconds)
{
	if (!read_from_file)
		return;

	update_time_elapsed += seconds;

	if (update_time_elapsed >= 1.0f) {
		time_t t = get_modified_timestamp(file.c_str());
		update_time_elapsed = 0.0f;

		if (update_file) {
			LoadFileText();
			TransformText();
			RenderText();
			update_file = false;
		}

		if (file_timestamp != t) {
			file_timestamp = t;
			update_file = true;
		}
	}
}

void TextSource::Render()
{
	if (!tex)
		return;

	gs_effect_t* effect = obs_get_base_effect(OBS_EFFECT_DEFAULT);
	gs_technique_t* tech = gs_effect_get_technique(effect, "Draw");

	gs_technique_begin(tech);
	gs_technique_begin_pass(tech, 0);

	gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"),
						  tex);
	gs_draw_sprite(tex, 0, cx, cy);

	gs_technique_end_pass(tech);
	gs_technique_end(tech);
}

#pragma endregion