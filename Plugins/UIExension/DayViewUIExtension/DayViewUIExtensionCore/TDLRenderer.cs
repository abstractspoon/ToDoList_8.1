using System;
using System.Collections.Generic;
using System.Text;
using System.Globalization;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Windows.Forms.VisualStyles;
using System.Windows.Forms;

using Abstractspoon.Tdl.PluginHelpers;
using Abstractspoon.Tdl.PluginHelpers.ColorUtil;

namespace DayViewUIExtension
{
    class TDLRenderer : Calendar.AbstractRenderer
    {
		private UIExtension.TaskIcon m_TaskIcons;
		private IntPtr m_hWnd;
		private Font m_BaseFont;
		private int m_ColWidth = -1;

		enum DOWNameStyle
		{
			None,
			Short,
			Long
		}
		private DOWNameStyle DOWStyle { get; set; }

		// ------------------------------------------------------------------------

		public TDLRenderer(IntPtr hWnd, UIExtension.TaskIcon taskIcons)
		{
			m_TaskIcons = taskIcons;
			m_hWnd = hWnd;

			ShowParentsAsFolder = false;
            TaskColorIsBackground = false;
            StrikeThruDoneTasks = true;
            GridlineColor = Color.Gray;
			DOWStyle = DOWNameStyle.Long;
		}

		public bool ShowParentsAsFolder { get; set; }
		public bool TaskColorIsBackground { get; set; }
        public bool StrikeThruDoneTasks { get; set; }

        public Color GridlineColor { get; set; }
        public UITheme Theme { get; set; }
		public int TextPadding { get { return 2; } }

        protected override void Dispose(bool mainThread)
        {
            base.Dispose(mainThread);

            if (m_BaseFont != null)
                m_BaseFont.Dispose();
        }

        public override Font BaseFont
        {
            get
            {
                if (m_BaseFont == null)
                {
                    m_BaseFont = new Font("Tahoma", 8, FontStyle.Regular);
                }

                return m_BaseFont;
            }
        }

		private void UpdateDOWStyle(Graphics g)
		{
			DOWStyle = DOWNameStyle.Long;

			// Subtract the width of the widest numerical component
			int colWidth = m_ColWidth;

			using (Font font = new Font(m_BaseFont, FontStyle.Bold))
			{
				colWidth -= (int)g.MeasureString("31", font).Width;
			}

			// Calculate the longest long and short day-of-week names
			int maxLong = DateUtil.GetMaxDayOfWeekNameWidth(g, m_BaseFont, false);
			int maxShort = DateUtil.GetMaxDayOfWeekNameWidth(g, m_BaseFont, true);

			if (maxLong > colWidth)
			{
				if (maxShort > colWidth)
					DOWStyle = DOWNameStyle.None;
				else
					DOWStyle = DOWNameStyle.Short;
			}
		}

		public override void SetColumnWidth(Graphics g, int colWidth)
		{
			if (m_ColWidth == colWidth)
				return;

			m_ColWidth = colWidth;

			// Update the visibility of the day of week component
			UpdateDOWStyle(g);
		}

		public void SetFont(String fontName, int fontSize)
        {
            if ((m_BaseFont.Name == fontName) && (m_BaseFont.Size == fontSize))
                return;

            m_BaseFont = new Font(fontName, fontSize, FontStyle.Regular);
 
			// Update the visibility of the day of week component
			using (Graphics g = Graphics.FromHwnd(m_hWnd))
			{
				UpdateDOWStyle(g);
			}
		}

		public int GetFontHeight()
        {
            return BaseFont.Height;
        }

        public override Color HourColor
        {
            get
            {
                return Color.FromArgb(230, 237, 247);
            }
        }

        public override Color HalfHourSeperatorColor
        {
            get
            {
                return GridlineColor;
            }
        }

        public override Color HourSeperatorColor
        {
            get
            {
				// Slightly darker
                return DrawingColor.AdjustLighting(GridlineColor, -0.2f, false);
            }
        }

        public override Color WorkingHourColor
        {
            get
            {
                return Color.FromArgb(255, 255, 255);
            }
        }

        public override Color BackColor
        {
            get
            {
				return Theme.GetAppDrawingColor(UITheme.AppColor.AppBackLight);
            }
        }

        public override Color SelectionColor
        {
            get
            {
                return Color.FromArgb(41, 76, 122);
            }
        }

        public Color TextColor
        {
            get
            {
                return Theme.GetAppDrawingColor(UITheme.AppColor.AppText);
            }
        }

		public override void DrawHourLabel(Graphics g, Rectangle rect, int hour, bool ampm)
        {
            if (g == null)
                throw new ArgumentNullException("g");

            using (SolidBrush brush = new SolidBrush(this.TextColor))
            {
				// Ignore 'ampm' and format for the current regional settings
                string amPmTime = "00";

                if (!String.IsNullOrEmpty(DateTimeFormatInfo.CurrentInfo.AMDesignator))
				{
					if (hour < 12)
						amPmTime = DateTimeFormatInfo.CurrentInfo.AMDesignator;
					else
						amPmTime = DateTimeFormatInfo.CurrentInfo.PMDesignator;

					if (hour != 12)
						hour = hour % 12;
				}

				String hourStr = hour.ToString("##00", System.Globalization.CultureInfo.InvariantCulture);
                
				g.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;
				g.DrawString(hourStr, HourFont, brush, rect);

                rect.X += ((int)g.MeasureString(hourStr, HourFont).Width + 2);

                g.DrawString(amPmTime, MinuteFont, brush, rect);
				g.TextRenderingHint = TextRenderingHint.SystemDefault;
			}
        }

        public override void DrawMinuteLine(Graphics g, Rectangle rect, int minute)
        {
            if (g == null)
                throw new ArgumentNullException("g");

            if ((minute % 30) == 0)
            {
                using (Pen pen = new Pen(MinuteLineColor))
                {
					g.SmoothingMode = SmoothingMode.None;

                    if (minute == 0)
                    {
                        g.DrawLine(pen, rect.Left, rect.Y, rect.Right, rect.Y);
                    }
                    else if (rect.Height > MinuteFont.Height)
                    {
                        // 30 min mark - halve line width
                        rect.X += rect.Width / 2;
                        rect.Width /= 2;

						g.DrawLine(pen, rect.Left, rect.Y, rect.Right, rect.Y);

                        // Draw label beneath
                        using (SolidBrush brush = new SolidBrush(this.TextColor)) 
                        {
                            g.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;
                            g.DrawString("30", MinuteFont, brush, rect);
                            g.TextRenderingHint = TextRenderingHint.SystemDefault;
                        }
                    }
                }
            }
        }

        private Color MinuteLineColor
        {
            get
            {
                Color appLineColor = Theme.GetAppDrawingColor(UITheme.AppColor.AppLinesDark);

                if (appLineColor == BackColor)
                    appLineColor = Theme.GetAppDrawingColor(UITheme.AppColor.AppLinesLight);

                return appLineColor;
            }
        }

        public override void DrawDayHeader(Graphics g, Rectangle rect, DateTime date)
        {
            if (g == null)
                throw new ArgumentNullException("g");

			// Header background
			bool isToday = date.Date.Equals(DateTime.Now.Date);

			Rectangle rHeader = rect;
			rHeader.X++;

			if (VisualStyleRenderer.IsSupported)
			{
				bool hasTodayColor = Theme.HasAppColor(UITheme.AppColor.Today);
				var headerState = VisualStyleElement.Header.Item.Normal;

				if (isToday && !hasTodayColor)
					headerState = VisualStyleElement.Header.Item.Hot;

				var renderer = new VisualStyleRenderer(headerState);
				renderer.DrawBackground(g, rHeader);

				if (isToday && hasTodayColor)
				{
					rHeader.X--;

					using (var brush = new SolidBrush(Theme.GetAppDrawingColor(UITheme.AppColor.Today, 64)))
						g.FillRectangle(brush, rHeader);
				}
			}
			else // classic theme
			{
				rHeader.Y++;

				var headerBrush = (isToday ? SystemBrushes.ButtonHighlight : SystemBrushes.ButtonFace);
                g.FillRectangle(headerBrush, rHeader);

				ControlPaint.DrawBorder3D(g, rHeader, Border3DStyle.Raised);
            }

			// Header text
			g.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;

			// Day of month
			var fmt = new StringFormat();

			fmt.LineAlignment = StringAlignment.Center;
			fmt.Alignment = StringAlignment.Near;
			fmt.FormatFlags |= StringFormatFlags.NoWrap;

			using (Font font = new Font(m_BaseFont, FontStyle.Bold))
			{
				if (DOWStyle == DOWNameStyle.None)
					fmt.Alignment = StringAlignment.Center;

				string dayNum = date.Day.ToString();
				g.DrawString(dayNum, font, SystemBrushes.WindowText, rect, fmt);

				if (DOWStyle == DOWNameStyle.Long)
				{
					int strWidth = (int)g.MeasureString(dayNum, font).Width;

					rect.Width -= strWidth;
					rect.X += strWidth;
				}
			}

			// Day of week
			if (DOWStyle != DOWNameStyle.None)
			{
				if (DOWStyle == DOWNameStyle.Long)
					fmt.Alignment = StringAlignment.Center;
				else
					fmt.Alignment = StringAlignment.Far;

				string dayName;

				if (DOWStyle == DOWNameStyle.Long)
					dayName = System.Globalization.CultureInfo.CurrentCulture.DateTimeFormat.GetDayName(date.DayOfWeek);
				else
					dayName = System.Globalization.CultureInfo.CurrentCulture.DateTimeFormat.GetAbbreviatedDayName(date.DayOfWeek);

				g.DrawString(dayName, m_BaseFont, SystemBrushes.WindowText, rect, fmt);
			}
		}

		public override void DrawDayBackground(Graphics g, Rectangle rect)
        {
            //using (SolidBrush backBrush = new SolidBrush(Theme.GetAppDrawingColor(UITheme.AppColor.AppBackDark)))
            //    g.FillRectangle(backBrush, rect);
        }

		public override void DrawHourRange(Graphics g, Rectangle rect, bool drawBorder, bool hilight)
		{
			if (hilight)
			{
				// Draw selection rect
				UIExtension.SelectionRect.Draw(m_hWnd, 
												g, 
												rect.X, 
												rect.Y, 
												rect.Width, 
												rect.Height, 
												UIExtension.SelectionRect.Style.SelectedNotFocused, 
												true); // transparent
			}
			else
			{
				base.DrawHourRange(g, rect, drawBorder, hilight);
			}
		}

		public bool TaskHasIcon(CalendarItem taskItem)
		{
			return ((m_TaskIcons != null) &&
					(taskItem != null) &&
					(taskItem.HasIcon || (ShowParentsAsFolder && taskItem.IsParent)));
		}

		private UInt32 GetRealTaskId(Calendar.Appointment appt)
		{
			if (appt is CalendarFutureItem)
				return (appt as CalendarFutureItem).RealTaskId;

			return appt.Id;
		}

		public override void DrawAppointment(Graphics g, Rectangle rect, Calendar.Appointment appointment, bool isLong, bool isSelected, Rectangle gripRect)
        {
            if (appointment == null)
                throw new ArgumentNullException("appointment");

            if (g == null)
                throw new ArgumentNullException("g");

            if (rect.Width != 0 && rect.Height != 0)
            {
				CalendarItem taskItem = (appointment as CalendarItem);

				UInt32 taskId = taskItem.Id;
				UInt32 realTaskId = GetRealTaskId(taskItem);

				bool isFutureItem = (taskId != realTaskId);

				// Recalculate colours
				Color textColor = taskItem.TaskTextColor;
				Color fillColor = DrawingColor.SetLuminance(textColor, 0.95f);

				if (isFutureItem)
				{
					fillColor = SystemColors.Window;

					float textLum = DrawingColor.GetLuminance(textColor);
					textColor = DrawingColor.SetLuminance(textColor, Math.Min(textLum + 0.2f, 0.7f));
				}

				Color borderColor = textColor;
				Color barColor = textColor;

				if (taskItem.HasTaskTextColor)
				{
					if (isSelected)
					{
						textColor = DrawingColor.SetLuminance(textColor, 0.3f);
					}
					else if (TaskColorIsBackground && !taskItem.IsDoneOrGoodAsDone && !isFutureItem)
					{
						barColor = textColor;
						fillColor = textColor;

						borderColor = DrawingColor.AdjustLighting(textColor, -0.5f, true);
						textColor = DrawingColor.GetBestTextColor(textColor);
					}
				}

                // Draw the background of the appointment
                g.SmoothingMode = SmoothingMode.None;

                if (isSelected)
                {
                    if (isLong)
                        rect.Height++;

					if (isFutureItem)
					{
						UIExtension.SelectionRect.Draw(m_hWnd,
														g,
														rect.Left,
														rect.Top,
														rect.Width,
														rect.Height,
														UIExtension.SelectionRect.Style.DropHighlighted,
														false); // opaque
					}
					else
					{
						UIExtension.SelectionRect.Draw(m_hWnd,
														g,
														rect.Left,
														rect.Top,
														rect.Width,
														rect.Height,
														false); // opaque
					}
                }
                else
                {
                    using (SolidBrush brush = new SolidBrush(fillColor))
                        g.FillRectangle(brush, rect);

                    if (taskItem.DrawBorder)
                    {
						if (!isLong)
						{
							rect.Height--; // drawing with pen adds 1 to height
							rect.Width--;
						}

						using (Pen pen = new Pen(borderColor, 1))
						{
							if (isFutureItem)
								pen.DashStyle = DashStyle.Dash;

							g.DrawRectangle(pen, rect);
						}
					}
                }

                // Draw appointment icon
                bool hasIcon = false;
                taskItem.IconRect = Rectangle.Empty;

                if (TaskHasIcon(taskItem))
                {
                    Rectangle rectIcon;
                    int imageSize = DPIScaling.Scale(16);

                    if (isLong)
                    {
                        int yCentre = ((rect.Top + rect.Bottom + 1) / 2);
                        rectIcon = new Rectangle((rect.Left + TextPadding), (yCentre - (imageSize / 2)), imageSize, imageSize);
                    }
                    else
                    {
                        rectIcon = new Rectangle(rect.Left + TextPadding, rect.Top + TextPadding, imageSize, imageSize);
                    }

                    if (g.IsVisible(rectIcon) && m_TaskIcons.Get(realTaskId))
                    {
                        if (isLong)
                        {
                            rectIcon.X = (gripRect.Right + TextPadding);
                        }
                        else
                        {
                            gripRect.Y += (imageSize + TextPadding);
                            gripRect.Height -= (imageSize + TextPadding);
                        }

                        var clipRgn = g.Clip;

                        if (rect.Bottom < (rectIcon.Y + imageSize))
                            g.Clip = new Region(RectangleF.Intersect(rect, g.ClipBounds));

                        m_TaskIcons.Draw(g, rectIcon.X, rectIcon.Y);

                        g.Clip = clipRgn;

                        hasIcon = true;
                        taskItem.IconRect = rectIcon;

                        rect.Width -= (rectIcon.Right - rect.Left);
                        rect.X = rectIcon.Right;
                    }
                }

                // Draw gripper bar
                if (gripRect.Width > 0)
                {
                    using (SolidBrush brush = new SolidBrush(barColor))
                        g.FillRectangle(brush, gripRect);

                    if (!isLong)
                        gripRect.Height--; // drawing with pen adds 1 to height

                    // Draw gripper border
                    using (Pen pen = new Pen(DrawingColor.AdjustLighting(barColor, -0.5f, true), 1))
                        g.DrawRectangle(pen, gripRect);

                    if (!hasIcon)
                    {
                        rect.X = gripRect.Right;
                        rect.Width -= gripRect.Width;
                    }
                }

                // draw appointment text
                using (StringFormat format = new StringFormat())
                {
                    format.Alignment = StringAlignment.Near;
                    format.LineAlignment = (isLong ? StringAlignment.Center : StringAlignment.Near);

					if (isLong)
						format.FormatFlags |= (StringFormatFlags.NoClip | StringFormatFlags.NoWrap);

                    rect.Y += 3;

					if (isLong)
						rect.Height = m_BaseFont.Height;
					else
						rect.Height -= 3;

					taskItem.TextRect = rect;
                    g.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;

                    using (SolidBrush brush = new SolidBrush(textColor))
                    {
                        if (taskItem.IsDone && StrikeThruDoneTasks)
                        {
                            using (Font font = new Font(this.BaseFont, FontStyle.Strikeout))
                            {
                                g.DrawString(appointment.Title, font, brush, rect, format);
                            }
                        }
                        else
                        {
                            g.DrawString(appointment.Title, this.BaseFont, brush, rect, format);
                        }
                    }

                    g.TextRenderingHint = TextRenderingHint.SystemDefault;
                }
            }
        }
    }
}
