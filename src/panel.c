#include <TUIC/tuic.h>
#include <TUIC/backends/objects.h>
#include "image_inline.h"

TuiPanel tuiPanelCreate(TuiInstance instance, int pixel_width, int pixel_height)
{
	if (instance == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_INSTANCE, __func__);
		return NULL;
	}
	if (instance->IsDamaged == TUI_TRUE)
	{
		tuiDebugError(TUI_ERROR_DAMAGED_INSTANCE, __func__);
		return NULL;
	}
	if (pixel_width <= 0 || pixel_height <= 0)
	{
		tuiDebugError(TUI_ERROR_INVALID_PANEL_DIMENSIONS, __func__);
		return NULL;
	}

	TuiPanel panel = tuiAllocate(sizeof(TuiPanel_s));
	panel->Instance = instance;
	panel->FramebufferWidth = (size_t)pixel_width;
	panel->FramebufferHeight = (size_t)pixel_height;
	panel->ApiData = NULL;
	panel->Instance->PanelCreate(panel);
	panel->Instance->PanelCount++;
	return panel;
}

void tuiPanelDestroy(TuiPanel panel)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return;
	}

	panel->Instance->PanelDestroy(panel);
	panel->Instance->PanelCount--;
	tuiFree(panel);
}

TuiInstance tuiPanelGetInstance(TuiPanel panel)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return NULL;
	}

	return panel->Instance;
}

TuiImage tuiPanelGetImage(TuiPanel panel, TuiImage fill_image)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return NULL;
	}
	if (panel->Instance->IsDamaged == TUI_TRUE)
	{
		tuiDebugError(TUI_ERROR_DAMAGED_INSTANCE, __func__);
		return NULL;
	}

	if (fill_image != NULL)
	{
		fill_image->PixelData = panel->Instance->PanelGetPixels(panel, &fill_image->PixelWidth, &fill_image->PixelHeight, fill_image->PixelData);
	}
	else
	{
		size_t p_width, p_height;
		uint8_t* pixel_data = panel->Instance->PanelGetPixels(panel, &p_width, &p_height, NULL);
		fill_image = create_image(p_width, p_height, 4, pixel_data, TUI_FALSE, __func__);
	}

	return fill_image;
}

uint8_t* tuiPanelGetPixels(TuiPanel panel, int* pixel_width, int* pixel_height, uint8_t* fill_pixels)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return NULL;
	}
	if (panel->Instance->IsDamaged == TUI_TRUE)
	{
		tuiDebugError(TUI_ERROR_DAMAGED_INSTANCE, __func__);
		return NULL;
	}

	size_t o_width, o_height;
	fill_pixels = panel->Instance->PanelGetPixels(panel, &o_width, &o_height, fill_pixels);
	*pixel_width = (int)o_width;
	*pixel_height = (int)o_height;
	return fill_pixels;
} 


void tuiPanelClearColor(TuiPanel panel, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return;
	}
	if (panel->Instance->IsDamaged == TUI_TRUE)
	{
		tuiDebugError(TUI_ERROR_DAMAGED_INSTANCE, __func__);
		return;
	}

	panel->Instance->PanelClearColor(panel, r, g, b, a);
}

void tuiPanelSetPixelDimensions(TuiPanel panel, int pixel_width, int pixel_height)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return;
	}
	if (panel->Instance->IsDamaged == TUI_TRUE)
	{
		tuiDebugError(TUI_ERROR_DAMAGED_INSTANCE, __func__);
		return;
	}
	if (pixel_width <= 0 || pixel_height <= 0)
	{
		tuiDebugError(TUI_ERROR_INVALID_PANEL_DIMENSIONS, __func__);
		return;
	}

	panel->Instance->PanelSetSize(panel, (size_t)pixel_width, (size_t)pixel_height);
}

void tuiPanelGetPixelDimensions(TuiPanel panel, int* pixel_width, int* pixel_height)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return;
	}

	if (pixel_width != NULL)
	{
		*pixel_width = (int)panel->FramebufferWidth;
	}
	if (pixel_height != NULL)
	{
		*pixel_height = (int)panel->FramebufferHeight;
	}
}

int tuiPanelGetPixelWidth(TuiPanel panel)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return 0;
	}

	return panel->FramebufferWidth;
}

int tuiPanelGetPixelHeight(TuiPanel panel)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return 0;
	}

	return panel->FramebufferHeight;
}


void tuiPanelDrawBatch(TuiPanel panel, TuiGlyphAtlas atlas, TuiPalette palette, TuiBatch batch)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return;
	}
	if (panel->Instance->IsDamaged == TUI_TRUE)
	{
		tuiDebugError(TUI_ERROR_DAMAGED_INSTANCE, __func__);
		return;
	}
	if (atlas == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_GLYPH_ATLAS, __func__);
		return;
	}
	if (batch == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_BATCH, __func__);
		return;
	}
	if (tuiDetailHasPalette(batch->DetailMode) == TUI_TRUE && palette == NULL)
	{
		tuiDebugError(TUI_ERROR_PALETTE_REQUIRED, __func__);
		return;
	}
	if (atlas->Instance != panel->Instance)
	{
		tuiDebugError(TUI_ERROR_UNMATCHING_ATLAS_INSTANCE, __func__);
		return;
	}
	if (palette != NULL && atlas->Instance != panel->Instance)
	{
		tuiDebugError(TUI_ERROR_UNMATCHING_PALETTE_INSTANCE, __func__);
		return;
	}
	if (tuiDetailHasFlag(batch->DetailMode, TUI_LAYOUT_FLAG_SPARSE) == TUI_TRUE && batch->TileCount == 0)
	{
		return;
	}

	panel->Instance->PanelDrawBatchData(panel, atlas, palette, batch->DetailMode, batch->TilesWide, batch->TilesTall, batch->TileCount, batch->Data, 0, panel->FramebufferWidth, 0, panel->FramebufferHeight);
}

void tuiPanelDrawBatchData(TuiPanel panel, TuiGlyphAtlas atlas, TuiPalette palette, int detail_mode, int tiles_wide, int tiles_tall, size_t sparse_index, uint8_t* batch_data)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return;
	}
	if (panel->Instance->IsDamaged == TUI_TRUE)
	{
		tuiDebugError(TUI_ERROR_DAMAGED_INSTANCE, __func__);
		return;
	}
	if (batch_data == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_BATCH_DATA, __func__);
		return;
	}
	if (tiles_wide <= 0 || tiles_tall <= 0)
	{
		tuiDebugError(TUI_ERROR_INVALID_BATCH_DATA_DIMENSIONS, __func__);
		return;
	}
	if (atlas->Instance != panel->Instance)
	{
		tuiDebugError(TUI_ERROR_UNMATCHING_ATLAS_INSTANCE, __func__);
		return;
	}
	if (tuiDetailHasPalette(detail_mode) == TUI_TRUE && palette == NULL)
	{
		tuiDebugError(TUI_ERROR_PALETTE_REQUIRED, __func__);
		return;
	}
	if (palette != NULL && atlas->Instance != panel->Instance)
	{
		tuiDebugError(TUI_ERROR_UNMATCHING_PALETTE_INSTANCE, __func__);
		return;
	}
	if (tuiDetailHasFlag(detail_mode, TUI_LAYOUT_FLAG_SPARSE) == TUI_TRUE && sparse_index == 0)
	{
		return;
	}

	panel->Instance->PanelDrawBatchData(panel, atlas, palette, (size_t)detail_mode, (size_t)tiles_wide, (size_t)tiles_tall, sparse_index, batch_data, 0, panel->FramebufferWidth, 0, panel->FramebufferHeight);
}

void tuiPanelDrawBatchTransformed(TuiPanel panel, TuiGlyphAtlas atlas, TuiPalette palette, TuiBatch batch, int left_x, int right_x, int top_y, int bottom_y)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return;
	}
	if (panel->Instance->IsDamaged == TUI_TRUE)
	{
		tuiDebugError(TUI_ERROR_DAMAGED_INSTANCE, __func__);
		return;
	}
	if (atlas == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_GLYPH_ATLAS, __func__);
		return;
	}
	if (batch == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_BATCH, __func__);
		return;
	}
	if (tuiDetailHasPalette(batch->DetailMode) == TUI_TRUE && palette == NULL)
	{
		tuiDebugError(TUI_ERROR_PALETTE_REQUIRED, __func__);
		return;
	}
	if (atlas->Instance != panel->Instance)
	{
		tuiDebugError(TUI_ERROR_UNMATCHING_ATLAS_INSTANCE, __func__);
		return;
	}
	if (palette != NULL && atlas->Instance != panel->Instance)
	{
		tuiDebugError(TUI_ERROR_UNMATCHING_PALETTE_INSTANCE, __func__);
		return;
	}
	if (tuiDetailHasFlag(batch->DetailMode, TUI_LAYOUT_FLAG_SPARSE) == TUI_TRUE && batch->TileCount == 0)
	{
		return;
	}

	panel->Instance->PanelDrawBatchData(panel, atlas, palette, batch->DetailMode, batch->TilesWide, batch->TilesTall, batch->TileCount, batch->Data, left_x, right_x, top_y, bottom_y);
}

void tuiPanelDrawBatchDataTransformed(TuiPanel panel, TuiGlyphAtlas atlas, TuiPalette palette, int detail_mode, int tiles_wide, int tiles_tall, size_t sparse_index, uint8_t* batch_data, int left_x, int right_x, int top_y, int bottom_y)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return;
	}
	if (panel->Instance->IsDamaged == TUI_TRUE)
	{
		tuiDebugError(TUI_ERROR_DAMAGED_INSTANCE, __func__);
		return;
	}
	if (batch_data == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_BATCH_DATA, __func__);
		return;
	}
	if (tiles_wide <= 0 || tiles_tall <= 0)
	{
		tuiDebugError(TUI_ERROR_INVALID_PANEL_DIMENSIONS, __func__);
		return;
	}
	if (atlas->Instance != panel->Instance)
	{
		tuiDebugError(TUI_ERROR_UNMATCHING_ATLAS_INSTANCE, __func__);
		return;
	}
	if (palette != NULL && atlas->Instance != panel->Instance)
	{
		tuiDebugError(TUI_ERROR_UNMATCHING_PALETTE_INSTANCE, __func__);
		return;
	}
	if (tuiDetailHasFlag(detail_mode, TUI_LAYOUT_FLAG_SPARSE) == TUI_TRUE && sparse_index == 0)
	{
		return;
	}

	panel->Instance->PanelDrawBatchData(panel, atlas, palette, (size_t)detail_mode, (size_t)tiles_wide, (size_t)tiles_tall, sparse_index, batch_data, left_x, right_x, top_y, bottom_y);
}

void tuiPanelRender(TuiPanel panel)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return;
	}
	if (panel->Instance->IsDamaged == TUI_TRUE)
	{
		tuiDebugError(TUI_ERROR_DAMAGED_INSTANCE, __func__);
		return;
	}

	panel->Instance->PanelRender(panel, 0, (int)panel->Instance->PixelWidth, 0, (int)panel->Instance->PixelHeight);
}

void tuiPanelRenderTransformed(TuiPanel panel, int left_x, int right_x, int top_y, int bottom_y)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return;
	}
	if (panel->Instance->IsDamaged == TUI_TRUE)
	{
		tuiDebugError(TUI_ERROR_DAMAGED_INSTANCE, __func__);
		return;
	}

	panel->Instance->PanelRender(panel, left_x, right_x, top_y, bottom_y);
}

void tuiPanelRenderToPanel(TuiPanel panel, TuiPanel target_panel)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return;
	}
	if (panel->Instance->IsDamaged == TUI_TRUE)
	{
		tuiDebugError(TUI_ERROR_DAMAGED_INSTANCE, __func__);
		return;
	}
	if (target_panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_TARGET_PANEL, __func__);
		return;
	}
	if (panel->Instance != target_panel->Instance)
	{
		tuiDebugError(TUI_ERROR_UNMATCHING_PANEL_INSTANCE, __func__);
		return;
	}

	panel->Instance->PanelRenderToPanel(panel, target_panel, 0, (int)target_panel->FramebufferWidth, 0, (int)target_panel->FramebufferHeight);
}

void tuiPanelRenderToPanelTransformed(TuiPanel panel, TuiPanel target_panel, int left_x, int right_x, int top_y, int bottom_y)
{
	if (panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_PANEL, __func__);
		return;
	}
	if (panel->Instance->IsDamaged == TUI_TRUE)
	{
		tuiDebugError(TUI_ERROR_DAMAGED_INSTANCE, __func__);
		return;
	}
	if (target_panel == NULL)
	{
		tuiDebugError(TUI_ERROR_NULL_TARGET_PANEL, __func__);
		return;
	}
	if (panel->Instance != target_panel->Instance)
	{
		tuiDebugError(TUI_ERROR_UNMATCHING_PANEL_INSTANCE, __func__);
		return;
	}

	panel->Instance->PanelRenderToPanel(panel, target_panel, left_x, right_x, top_y, bottom_y);
}
