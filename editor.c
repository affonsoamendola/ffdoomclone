
VECTOR2 editor_center;
VECTOR2 editor_cursor;

float editor_zoom = 3.;

void move_view(VECTOR2 amount)
{
	editor_center = sum_v2(editor_center, amount);
	editor_cursor = sum_v2(editor_cursor, amount);
}

void set_zoom(float zoom)
{
	editor_zoom = zoom;
}