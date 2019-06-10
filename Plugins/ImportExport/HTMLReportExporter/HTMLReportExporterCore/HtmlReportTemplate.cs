﻿using System;
using System.IO;
using System.Collections.Generic;
using System.Xml.Linq;
using System.Text;
using System.Net;
using System.Xml;
using System.Drawing;
using System.Web.UI;

using Abstractspoon.Tdl.PluginHelpers;

namespace HTMLReportExporter
{
	public class TemplateItem
	{
		protected String XmlTag;

		// ----------------------------------

		public bool Enabled { set; get; }
		public String Text { set; get; }
		public String EnabledText { get { return (Enabled ? Text : ""); } }
		
		// ----------------------------------

		public TemplateItem(String xmlTag)
		{
			Clear();

			XmlTag = xmlTag;
		}

		virtual public void Clear()
		{
			Enabled = true;
			Text = "";
		}

		virtual public bool Equals(TemplateItem other)
		{
			return ((other != null) && (Enabled == other.Enabled) && Text.Equals(other.Text));
		}

		virtual public bool Copy(TemplateItem other)
		{
			if (other == null)
				return false;

			Enabled = other.Enabled;
			Text = String.Copy(other.Text);
			XmlTag = String.Copy(other.XmlTag);

			return true;
		}

		virtual public void Read(XDocument doc)
		{
			Text = ReadValue(doc, "text");
			Enabled = ReadValue(doc, "enabled", true);
		}

		virtual public void Write(XDocument doc)
		{
			doc.Root.Add(new XElement(XmlTag,
							new XElement("text", Text),
							new XElement("enabled", Enabled)));
		}

		protected String ReadValue(XDocument doc, String element, String defValue = "")
		{
			var xElm = doc.Root.Element(XmlTag).Element(element);

			return ((xElm == null) ? defValue : xElm.Value);
		}

		protected int ReadValue(XDocument doc, String element, int defValue)
		{
			var xElm = doc.Root.Element(XmlTag).Element(element);

			return ((xElm == null) ? defValue : (int)xElm);
		}

		protected bool ReadValue(XDocument doc, String element, bool defValue)
		{
			var xElm = doc.Root.Element(XmlTag).Element(element);

			return ((xElm == null) ? defValue : (bool)xElm);
		}
	}

	//////////////////////////////////////////////////////////////////////////////

	public class HeaderFooterTemplateItem : TemplateItem
	{
		private int DefPixelHeight = 0;

		// ----------------------------------

		public bool WantDivider { get; set; }
		public Color BackColor { get; set; }
		public int PixelHeight { get; set; }

		public String PixelHeightText
		{
			get { return PixelHeight.ToString(); }
			set
			{
				int height = 0;

				if (Int32.TryParse(value, out height) && (height > 0))
					PixelHeight = height;
			}
		}

		// ----------------------------------

		public HeaderFooterTemplateItem(String xmlTag, int defPixelHeight) : base(xmlTag)
		{
			DefPixelHeight = defPixelHeight;

			Clear();
		}

		public String BackColorHtml
		{
			get
			{
				return ColorTranslator.ToHtml(Color.FromArgb(BackColor.ToArgb()));
			}

			set
			{
				if (String.IsNullOrWhiteSpace(value))
					BackColor = Color.White;
				else
					BackColor = ColorTranslator.FromHtml(value);
			}
		}

		override public void Read(XDocument doc)
		{
			base.Read(doc);

			WantDivider = ReadValue(doc, "wantDivider", true);
			BackColorHtml = ReadValue(doc, "backColor");
			PixelHeight = ReadValue(doc, "pixelHeight", DefPixelHeight);
		}

		override public void Write(XDocument doc)
		{
			base.Write(doc);

			doc.Root.Element(XmlTag).Add(new XElement("wantDivider", WantDivider));
			doc.Root.Element(XmlTag).Add(new XElement("pixelHeight", PixelHeight));

			if ((BackColor != Color.Transparent) && (BackColor != Color.White))
				doc.Root.Element(XmlTag).Add(new XElement("backColor", BackColorHtml));
		}

		override public void Clear()
		{
			base.Clear();

			WantDivider = true;
			BackColor = Color.White;
			PixelHeight = DefPixelHeight;
		}

		public bool Equals(HeaderFooterTemplateItem other)
		{
			return (base.Equals(other) && 
					(WantDivider == other.WantDivider) &&
					(PixelHeight == other.PixelHeight) &&
					(BackColor.ToArgb() == other.BackColor.ToArgb()));
		}

		public bool Copy(HeaderFooterTemplateItem other)
		{
			if (!base.Copy(other))
				return false;

			WantDivider = other.WantDivider;
			PixelHeight = other.PixelHeight;
			BackColor = Color.FromArgb(other.BackColor.ToArgb());

			return true;
		}

	}

	//////////////////////////////////////////////////////////////////////////////

	public class HeaderTemplate : HeaderFooterTemplateItem
	{
		public HeaderTemplate() : base("header", 100)
		{
		}

		public HeaderTemplate(HeaderTemplate header) : this()
		{
			Copy(header);
		}

	}

	//////////////////////////////////////////////////////////////////////////////

	public class FooterTemplate : HeaderFooterTemplateItem
	{
		public FooterTemplate() : base("footer", 50)
		{
		}

		public FooterTemplate(FooterTemplate footer) : this()
		{
			Copy(footer);
		}

	}

	//////////////////////////////////////////////////////////////////////////////

	public class TitleTemplate : TemplateItem
	{
		public bool SeparatePage { get; set; }

		// ----------------------------------

		public TitleTemplate() : base("title")
		{
		}

		override public void Read(XDocument doc)
		{
			base.Read(doc);

			SeparatePage = ReadValue(doc, "separatePage", true);
		}

		override public void Write(XDocument doc)
		{
			base.Write(doc);

			doc.Root.Element(XmlTag).Add(new XElement("separatePage", SeparatePage));
		}

		override public void Clear()
		{
			base.Clear();

			SeparatePage = true;
		}

		public bool Equals(TitleTemplate other)
		{
			return (base.Equals(other) && (SeparatePage == other.SeparatePage));
		}

		public bool Copy(TitleTemplate other)
		{
			if (!base.Copy(other))
				return false;

			SeparatePage = other.SeparatePage;
			return true;
		}

	}

	//////////////////////////////////////////////////////////////////////////////

	public class TaskTemplate : TemplateItem
	{
		public struct TaskTemplateAttribute
		{
			public TaskTemplateAttribute(Task.Attribute id, String label, String placeHolder)
			{
				Id = id;
				Label = label;
				PlaceHolder = placeHolder;
			}

			public Task.Attribute Id;
			public String Label;
			public String PlaceHolder;
		}

		// -----------------------------------------------------------

		public static TaskTemplateAttribute[] Attributes =
		{
			new TaskTemplateAttribute(Task.Attribute.AllocatedBy,       "Allocated By",             "$(allocBy)" ),
			new TaskTemplateAttribute(Task.Attribute.AllocatedTo,       "Allocated To",             "$(allocTo)" ),
			new TaskTemplateAttribute(Task.Attribute.Category,          "Category",                 "$(cat)" ),
			new TaskTemplateAttribute(Task.Attribute.Cost,              "Cost",                     "$(cost)" ),
			new TaskTemplateAttribute(Task.Attribute.CreatedBy,         "Created By",               "$(createBy)" ),
			new TaskTemplateAttribute(Task.Attribute.CreationDate,      "Creation Date",            "$(createDate)" ),
			new TaskTemplateAttribute(Task.Attribute.Dependency,        "Dependency",               "$(depends)" ),
			new TaskTemplateAttribute(Task.Attribute.DoneDate,          "Completion Date",          "$(doneDate)" ),
			new TaskTemplateAttribute(Task.Attribute.DueDate,           "Due Date",                 "$(dueDate)" ),
			new TaskTemplateAttribute(Task.Attribute.ExternalId,        "ExternalId",               "$(extId)" ),
			new TaskTemplateAttribute(Task.Attribute.FileReference,     "File Link",                "$(filelink)" ),
			new TaskTemplateAttribute(Task.Attribute.Flag,              "Flag",                     "$(flag)" ),
			new TaskTemplateAttribute(Task.Attribute.HtmlComments,      "Comments",                 "$(comments)" ),
			new TaskTemplateAttribute(Task.Attribute.Id,                "Id",                       "$(id)" ),
			new TaskTemplateAttribute(Task.Attribute.LastModifiedBy,    "Last Modified By",         "$(modBy)" ),
			new TaskTemplateAttribute(Task.Attribute.LastModifiedDate,  "Last Modified Date",       "$(modDate)" ),
			new TaskTemplateAttribute(Task.Attribute.ParentId,          "Parent Id",                "$(pid)" ),
			new TaskTemplateAttribute(Task.Attribute.Path,              "Path",                     "$(path)" ),
			new TaskTemplateAttribute(Task.Attribute.Percent,           "Percentage Completion",    "$(percent)" ),
			new TaskTemplateAttribute(Task.Attribute.Position,          "Position",                 "$(pos)" ),
			new TaskTemplateAttribute(Task.Attribute.Priority,          "Priority",                 "$(priority)" ),
			new TaskTemplateAttribute(Task.Attribute.Recurrence,        "Recurrence",               "$(recurs)" ),
			new TaskTemplateAttribute(Task.Attribute.Risk,              "Risk",                     "$(risk)" ),
			new TaskTemplateAttribute(Task.Attribute.StartDate,         "Start Date",               "$(startDate)" ),
			new TaskTemplateAttribute(Task.Attribute.Status,            "Status",                   "$(status)" ),
			new TaskTemplateAttribute(Task.Attribute.SubtaskDone,       "Subtask Done",             "$(subtaskDone)" ),
			new TaskTemplateAttribute(Task.Attribute.Tags,              "Tags",                     "$(tag)" ),
			new TaskTemplateAttribute(Task.Attribute.TimeEstimate,      "Time Estimate",            "$(est)" ),
			new TaskTemplateAttribute(Task.Attribute.TimeSpent,         "Time Spent",               "$(spent)" ),
			new TaskTemplateAttribute(Task.Attribute.Title,             "Title",                    "$(title)" ),
			new TaskTemplateAttribute(Task.Attribute.Version,           "Version",                  "$(ver)" ),
		};

		// -----------------------------------------------------------

		public class TaskTemplateLayout
		{
			public StyleType Style;
			public String StartHtml, TaskHtml, EndHtml;

			// ------------------------------------------------------

			public enum StyleType
			{
				None,
				Table,
				OrderedList,
				UnorderedList
			}

			// ------------------------------------------------------

			public TaskTemplateLayout(String text)
			{
				InitLayout(text);
			}

			public void Clear()
			{
				Style = StyleType.None;
				TaskHtml = String.Empty;
				StartHtml = String.Empty;
				EndHtml = String.Empty;
			}

			public void InitLayout(string text)
			{
				Clear();

				if (String.IsNullOrWhiteSpace(text))
					return;

				try
				{
					TaskHtml = text; // default

					var doc = new HtmlAgilityPack.HtmlDocument();
					doc.LoadHtml(text);

					// Remove everything before the first bit of text
					// or the first major structural element
					var elm = doc.DocumentNode.FirstChild;

					while (elm != null)
					{
						// get next sibling in case we need to delete this node
						var nextElm = elm.NextSibling;

						if (AgilityUtils.ElementHasContent(elm))
							break;

						elm.Remove();
						elm = nextElm;
					}

					if (elm != null)
					{
						switch (elm.Name.ToUpper())
						{
							case "TABLE":
								this.Style = TaskTemplateLayout.StyleType.Table;
								this.TaskHtml = AgilityUtils.GetElementInnerHtml(elm, "TBODY");
								break;

							case "UL":
								this.Style = TaskTemplateLayout.StyleType.UnorderedList;
								this.TaskHtml = elm.InnerHtml;
								break;

							case "OL":
								this.Style = TaskTemplateLayout.StyleType.OrderedList;
								this.TaskHtml = elm.InnerHtml;
								break;
						}

						if (this.Style != TaskTemplateLayout.StyleType.None)
						{
							int taskStart = elm.OuterHtml.IndexOf(this.TaskHtml);

							this.StartHtml = elm.OuterHtml.Substring(0, taskStart);
							this.EndHtml = elm.OuterHtml.Substring(taskStart + this.TaskHtml.Length);

							if (this.Style == TaskTemplateLayout.StyleType.Table)
							{
								// Prefix 'Table:Tbody' with header row
								int tbodyStart = this.StartHtml.ToUpper().IndexOf("<TBODY>");

								if (tbodyStart != -1)
								{
									var theadStyle = "style=font-weight:bold;font-size:1.5em;display:table-header-group;";
									var theadHtml = String.Format("\n<thead {0}>{1}</thead>", theadStyle, FormatHeader());

									this.StartHtml = this.StartHtml.Insert(tbodyStart, theadHtml);
								}

								// Wrap <td> in <div> to prevent breaks across page
								//this.TaskHtml = this.TaskHtml.Replace("<td>", "<td><div class=\"avoid\">");
								//this.TaskHtml = this.TaskHtml.Replace("</td>", "</div></td>");

							}

						}
					}
				}
				catch
				{
					this.Clear();
				}
			}

			private String FormatHeader()
			{
				var header = TaskHtml;

				if (!String.IsNullOrWhiteSpace(header))
				{
					foreach (var attrib in Attributes)
						header = header.Replace(attrib.PlaceHolder, attrib.Label);
				}

				return header;
			}

		}


		// -----------------------------------------------------------

		public TaskTemplate() : base("task")
		{
		}


	}

	//////////////////////////////////////////////////////////////////////////////

	public class HtmlReportTemplate
	{
		private String m_FilePath = String.Empty;

		// ---------------------------------------------

		public HtmlReportTemplate()
		{
			Clear();
		}

		public void Clear()
		{
			Header = new HeaderTemplate();
			Title = new TitleTemplate();
			Task = new TaskTemplate();
			Footer = new FooterTemplate();
		}

		public HtmlReportTemplate(String pathName) : this()
		{
			Load(pathName);
		}

		public HeaderTemplate Header { get; private set; }
		public TitleTemplate Title { get; private set; }
		public TaskTemplate Task { get; private set; }
		public FooterTemplate Footer { get; private set; }

		public bool Equals(HtmlReportTemplate other)
		{
			if (other == null)
				return false;

			return (Header.Equals(other.Header) &&
					Title.Equals(other.Title) &&
					Task.Equals(other.Task) &&
					Footer.Equals(other.Footer));
		}

		public bool Copy(HtmlReportTemplate other)
		{
			if (other == null)
				return false;

			Header.Copy(other.Header);
			Title.Copy(other.Title);
			Task.Copy(other.Task);
			Footer.Copy(other.Footer);

			return true;
		}

		public bool Load(String pathName)
		{
			if (!File.Exists(pathName))
				return false;

			// else
			Clear();

			try
			{
				var doc = XDocument.Load(pathName);

				Header.Read(doc);
				Title.Read(doc);
				Task.Read(doc);
				Footer.Read(doc);
			}
			catch
			{
				Clear();
				return false;
			}

			m_FilePath = pathName;
			return true;
		}

		public bool HasContents()
		{
			return !Equals(new HtmlReportTemplate());
		}

		public bool Save()
		{
			return Save(m_FilePath);
		}

		public bool Save(String pathName)
		{
			try
			{
				XDocument doc = new XDocument(new XElement("ReportWriterTemplate"));

				Header.Write(doc);
				Title.Write(doc);
				Task.Write(doc);
				Footer.Write(doc);

				doc.Save(pathName);
			}
			catch
			{
				return false;
			}

			m_FilePath = pathName;
			return true;
		}

	}
}
