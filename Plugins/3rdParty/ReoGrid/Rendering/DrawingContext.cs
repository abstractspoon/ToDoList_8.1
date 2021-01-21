﻿/*****************************************************************************
 * 
 * ReoGrid - .NET Spreadsheet Control
 * 
 * http://reogrid.net/
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * Author: Jing <lujing at unvell.com>
 *
 * Copyright (c) 2012-2016 Jing <lujing at unvell.com>
 * Copyright (c) 2012-2016 unvell.com, all rights reserved.
 * 
 ****************************************************************************/

using unvell.ReoGrid.Views;
using unvell.ReoGrid.Graphics;

using RGFloat = System.Single;

namespace unvell.ReoGrid.Rendering
{
	/// <summary>
	/// Drawing Mode for render grid control
	/// </summary>
	public enum DrawMode
	{
		/// <summary>
		/// View on screen
		/// </summary>
		View,

		/// <summary>
		/// Print preview 
		/// </summary>
		Preview,

		/// <summary>
		/// Print
		/// </summary>
		Print,
	}

	#region DrawingContext
	/// <summary>
	/// Represents the platform no-associated drawing context.
	/// </summary>
	public abstract class DrawingContext
	{
		/// <summary>
		/// Get current instance of worksheet.
		/// </summary>
		public Worksheet Worksheet { get; private set; }

		/// <summary>
		/// Platform independent drawing context.
		/// </summary>
		public IGraphics Graphics { get; internal set; }

		internal IRenderer Renderer { get { return (IRenderer)Graphics; } }

		internal IView CurrentView { get; set; }

		public bool Focused { get; set; }

		/// <summary>
		/// Draw mode that decides what kind of content will be drawn during this drawing event.
		/// </summary>
		public DrawMode DrawMode { get; private set; }

		//internal DrawingContext(Worksheet worksheet, DrawMode drawMode)
		//	: this(worksheet, drawMode, null)
		//{
		//}

		internal DrawingContext(Worksheet worksheet, DrawMode drawMode, IRenderer r, bool focused)
		{
			this.Worksheet = worksheet;
			this.DrawMode = drawMode;
			this.Graphics = r;
			this.Focused = focused;
		}
	}
	#endregion // DrawingContext

	#region CellDrawingContext
	/// <summary>
	/// Drawing context for rendering cells.
	/// </summary>
	public sealed class CellDrawingContext : DrawingContext
	{
#region Cell Methods

		/// <summary>
		/// Cell instance if enter a cell drawing event
		/// </summary>
		public Cell Cell { get; set; }

		internal bool AllowCellClip { get; set; }
		
		internal bool FullCellClip { get; set; }

		/// <summary>
		/// Recall core renderer to draw cell text
		/// </summary>
		public void DrawCellText()
		{
			if (CurrentView is CellsViewport
				&& Cell != null
				&& !string.IsNullOrEmpty(Cell.DisplayText))
			{
				var view = ((CellsViewport)CurrentView);

				var g = this.Graphics;

				RGFloat scaleFactor = Worksheet.renderScaleFactor;

				g.PopTransform();

				view.DrawCellText(this, Cell);

				g.PushTransform();
				if (scaleFactor != 1f) g.ScaleTransform(scaleFactor, scaleFactor);
				g.TranslateTransform(this.Cell.Left, this.Cell.Top);
			}
		}

		/// <summary>
		/// Recall core renderer to draw cell background.
		/// </summary>
		public void DrawCellBackground()
		{
			if (this.CurrentView is CellsViewport 
				&& Cell != null)
			{
				var currentView = (CellsViewport)this.CurrentView;

				currentView.DrawCellBackground(this, Cell.InternalRow, Cell.InternalCol, Cell, true);
			}
		}

#endregion // Cell Methods

		internal CellDrawingContext(Worksheet worksheet, DrawMode drawMode)
			: this(worksheet, drawMode, null, false)
		{
		}

		internal CellDrawingContext(Worksheet worksheet, DrawMode drawMode, IRenderer r, bool focused)
			: base(worksheet, drawMode, r, focused)
		{
			this.AllowCellClip = !worksheet.HasSettings(WorksheetSettings.View_AllowCellTextOverflow);
		}
	}
	#endregion // CellDrawingContext

}

namespace unvell.ReoGrid
{
	[System.Obsolete("use unvell.ReoGrid.Rendering.CellDrawingContext instead")]
	public sealed class RGDrawingContext
	{
	}
}