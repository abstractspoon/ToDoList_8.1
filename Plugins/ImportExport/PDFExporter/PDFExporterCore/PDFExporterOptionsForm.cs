﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Text.RegularExpressions;
using System.Drawing;
using System.Drawing.Text;
using System.IO;
using System.Windows.Forms;
using System.Runtime.InteropServices;

using Microsoft.Win32;

namespace PDFExporter
{
	public partial class PDFExporterOptionsForm : Form
	{
		public PDFExporterOptionsForm(string installedFontPath, string otherFontPath, bool useOtherFont, string bkgndImagePath)
		{
			InitializeComponent();

			BuildFontCombo(installedFontPath);

			comboFont.Enabled = !useOtherFont;
			checkOtherFont.Checked = useOtherFont;
			editOtherFont.Enabled = useOtherFont;
			editOtherFont.Text = otherFontPath;
			btnBrowseOtherFont.Enabled = useOtherFont;
			editWatermarkImage.Text = bkgndImagePath;

			UpdateOKButton();
		}

		public string InstalledFontPath
		{
			get
			{
				var font = (comboFont.SelectedItem as FontItem);

				if (font == null)
					return String.Empty;

				return font.File;
			}
		}

		public string OtherFontPath { get { return editOtherFont.Text; } }
		public bool UseOtherFont { get { return (checkOtherFont.Checked && !string.IsNullOrWhiteSpace(OtherFontPath)); } }
		public string SelectedFontPath { get { return (UseOtherFont ? OtherFontPath : InstalledFontPath); } }
		public string WatermarkImagePath { get { return editWatermarkImage.Text; } }

		private class FontItem
		{
			public FontItem(string name, string file)
			{
				Name = name;
				File = file;
			}

			public override string ToString()
			{
				return Name;
			}

			public string Name;
			public string File;
		}

		private void BuildFontCombo(string selFontPath)
		{
			// iTextSharp only supports .ttf files
			var fontFolder = Environment.GetFolderPath(Environment.SpecialFolder.Fonts);
			var fontFiles = Directory.GetFiles(fontFolder, "*.ttf");

			// Avoid duplicates
			var addedFonts = new HashSet<string>();
			
			foreach (var fontFile in fontFiles)
			{
				var fontName = GetFontFromFileName(fontFile);

				if (!string.IsNullOrEmpty(fontName) && !addedFonts.Contains(fontName))
				{
					var fontItem = new FontItem(fontName, fontFile);
					comboFont.Items.Add(fontItem);

					if (String.Compare(fontFile, selFontPath, true) == 0)
						comboFont.SelectedItem = fontItem;

					addedFonts.Add(fontName);
				}
			}
		}

		// ------------------------------------------------------------------
		[DllImport("gdi32.dll", CharSet = CharSet.Auto, SetLastError = true)]
		public static extern int RemoveFontResourceEx(string lpszFilename, int fl, IntPtr pdv);

		private static int FR_PRIVATE = 16;
		// ------------------------------------------------------------------

		public static string GetFontFromFileName(string fileName)
		{
			var fontName = string.Empty;
			var fc = new PrivateFontCollection();

			try
			{
				fc.AddFontFile(fileName);
				fontName = fc.Families[0].Name;
			}
			catch (FileNotFoundException)
			{
			}

			fc.Dispose();

			// There is a bug in PrivateFontCollection::Dispose()
			// which does not release the font file in GDI32.dll
			// This results in duplicate font names for anyone 
			// calling the Win32 function EnumFonts.
			RemoveFontResourceEx(fileName, FR_PRIVATE, IntPtr.Zero);

			return fontName;
		}

		public static string GetFileNameFromFont(string fontName, bool bold = false, bool italic = false)
		{
			RegistryKey fonts = null;
			string fontFile = String.Empty;

			try
			{
				fonts = Registry.LocalMachine.OpenSubKey(@"Software\Microsoft\Windows NT\CurrentVersion\Fonts", false);

				if (fonts == null)
					fonts = Registry.LocalMachine.OpenSubKey(@"Software\Microsoft\Windows\CurrentVersion\Fonts", false);

				if (fonts != null)
				{
					string suffix = "";

					if (bold)
						suffix += "(?: Bold)?";

					if (italic)
						suffix += "(?: Italic)?";

					var regex = new Regex(@"^(?:.+ & )?" + Regex.Escape(fontName) + @"(?: & .+)?(?<suffix>" + suffix + @") \(TrueType\)$", RegexOptions.Compiled);

					string[] names = fonts.GetValueNames();

					string name = names.Select(n => regex.Match(n)).Where(m => m.Success).OrderByDescending(m => m.Groups["suffix"].Length).Select(m => m.Value).FirstOrDefault();

					if (name != null)
						fontFile = fonts.GetValue(name).ToString();
				}
			}
			finally
			{
				if (fonts != null)
					fonts.Dispose();
			}

			if (!string.IsNullOrEmpty(fontFile))
				fontFile = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.Fonts), fontFile);

			return fontFile;
		}

		public static string GetFileNameFromFont(Font font)
		{
			return GetFileNameFromFont(font.Name, font.Bold, font.Italic);
		}

		private void OnCheckChangeOtherFont(object sender, EventArgs e)
		{
			bool otherFont = checkOtherFont.Checked;

			comboFont.Enabled = !otherFont;
			editOtherFont.Enabled = otherFont;
			btnBrowseOtherFont.Enabled = otherFont;

			UpdateOKButton();
		}

		private void OnBrowseOtherFont(object sender, EventArgs e)
		{
			var dlg = new OpenFileDialog
			{
				Title = this.Text,

				AutoUpgradeEnabled = true,
				CheckFileExists = true,
				CheckPathExists = true,
				Filter = "True Type Fonts (*.ttf)|*.ttf",
				FilterIndex = 0,
			};

			if (dlg.ShowDialog() == DialogResult.OK)
			{
				editOtherFont.Text = dlg.FileName;
			}
		}

		private void OnOtherFontChanged(object sender, EventArgs e)
		{
			if (File.Exists(editOtherFont.Text))
				editOtherFont.ForeColor = SystemColors.WindowText;
			else
				editOtherFont.ForeColor = Color.Red;

			UpdateOKButton();
		}

		private void UpdateOKButton()
		{
			btnOK.Enabled = (!UseOtherFont || File.Exists(editOtherFont.Text));
		}

		private void OnBrowseWatermarkImage(object sender, EventArgs e)
		{
			var dlg = new OpenFileDialog
			{
				Title = this.Text,

				AutoUpgradeEnabled = true,
				CheckFileExists = true,
				CheckPathExists = true,
				Filter = MSDN.Html.Editor.EnterImageForm.ImageFilter,
				FilterIndex = 0,
			};

			if (dlg.ShowDialog() == DialogResult.OK)
			{
				editWatermarkImage.Text = dlg.FileName;
			}
		}
	}
}
