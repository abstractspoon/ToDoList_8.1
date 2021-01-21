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

#if DRAWING

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using unvell.ReoGrid.Rendering;

namespace unvell.ReoGrid.Drawing
{
	/// <summary>
	/// Represents the floating objects drawing context.
	/// </summary>
	public class FloatingDrawingContext : unvell.ReoGrid.Rendering.DrawingContext
	{
		/// <summary>
		/// Get the current drawing object.
		/// </summary>
		public DrawingObject CurrentObject { get; private set; }

		internal FloatingDrawingContext(Worksheet worksheet, DrawMode drawMode, IRenderer r, bool focused)
			: base(worksheet, drawMode, r, focused)
		{
		}

		internal void SetCurrentObject(DrawingObject currentObject)
		{
			this.CurrentObject = currentObject;
		}
	}
}

#endif // DRAWING