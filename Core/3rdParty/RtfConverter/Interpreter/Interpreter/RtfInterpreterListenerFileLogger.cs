// -- FILE ------------------------------------------------------------------
// name       : RtfInterpreterListenerFileLogger.cs
// project    : RTF Framelet
// created    : Jani Giannoudis - 2008.06.03
// language   : c#
// environment: .NET 2.0
// copyright  : (c) 2004-2009 by Itenso GmbH, Switzerland
// --------------------------------------------------------------------------
using System;
using System.IO;
using System.Globalization;

namespace Itenso.Rtf.Interpreter
{

	// ------------------------------------------------------------------------
	public class RtfInterpreterListenerFileLogger : RtfInterpreterListenerBase, IDisposable
	{

		// ----------------------------------------------------------------------
		public const string DefaultLogFileExtension = ".interpreter.log";

		// ----------------------------------------------------------------------
		public RtfInterpreterListenerFileLogger( string fileName )
			: this( fileName, new RtfInterpreterLoggerSettings() )
		{
		} // RtfInterpreterListenerFileLogger

		// ----------------------------------------------------------------------
		public RtfInterpreterListenerFileLogger( string fileName, RtfInterpreterLoggerSettings settings )
		{
			if ( fileName == null )
			{
				throw new ArgumentNullException( "fileName" );
			}
			if ( settings == null )
			{
				throw new ArgumentNullException( "settings" );
			}

			this.fileName = fileName;
			this.settings = settings;
		} // RtfInterpreterListenerFileLogger

		// ----------------------------------------------------------------------
		public string FileName
		{
			get { return this.fileName; }
		} // FileName

		// ----------------------------------------------------------------------
		public RtfInterpreterLoggerSettings Settings
		{
			get { return this.settings; }
		} // Settings

		// ----------------------------------------------------------------------
		public virtual void Dispose()
		{
			CloseStream();
		} // Dispose

		// ----------------------------------------------------------------------
		protected override void DoBeginDocument( IRtfInterpreterContext context )
		{
			EnsureDirectory();
			OpenStream();

			if ( this.settings.Enabled && !string.IsNullOrEmpty( this.settings.BeginDocumentText ) )
			{
				WriteLine( this.settings.BeginDocumentText );
			}
		} // DoBeginDocument

		// ----------------------------------------------------------------------
		protected override void DoInsertText( IRtfInterpreterContext context, string text )
		{
			if ( this.settings.Enabled && !string.IsNullOrEmpty( this.settings.TextFormatText ) )
			{
				string msg = text;
				if ( msg.Length > this.settings.TextMaxLength && !string.IsNullOrEmpty( this.settings.TextOverflowText ) )
				{
					msg = msg.Substring( 0, msg.Length - this.settings.TextOverflowText.Length ) + this.settings.TextOverflowText;
				}
				WriteLine( string.Format(
					CultureInfo.InvariantCulture,
					this.settings.TextFormatText,
					msg,
					context.CurrentTextFormat ) );
			}
		} // DoInsertText

		// ----------------------------------------------------------------------
		protected override void DoInsertSpecialChar( IRtfInterpreterContext context, RtfVisualSpecialCharKind kind )
		{
			if ( this.settings.Enabled && !string.IsNullOrEmpty( this.settings.SpecialCharFormatText ) )
			{
				WriteLine( string.Format(
					CultureInfo.InvariantCulture,
					this.settings.SpecialCharFormatText,
					kind ) );
			}
		} // DoInsertSpecialChar

		// ----------------------------------------------------------------------
		protected override void DoInsertBreak( IRtfInterpreterContext context, RtfVisualBreakKind kind )
		{
			if ( this.settings.Enabled && !string.IsNullOrEmpty( this.settings.BreakFormatText ) )
			{
				WriteLine( string.Format(
					CultureInfo.InvariantCulture,
					this.settings.BreakFormatText,
					kind ) );
			}
		} // DoInsertBreak

		// ----------------------------------------------------------------------
		protected override void DoInsertImage( IRtfInterpreterContext context,
			RtfVisualImageFormat format,
			int width, int height, int desiredWidth, int desiredHeight,
			int scaleWidthPercent, int scaleHeightPercent,
			string imageDataHex
		)
		{
			if ( this.settings.Enabled && !string.IsNullOrEmpty( this.settings.ImageFormatText ) )
			{
				WriteLine( string.Format(
					CultureInfo.InvariantCulture,
					this.settings.ImageFormatText,
					format,
					width,
					height,
					desiredWidth,
					desiredHeight,
					scaleWidthPercent,
					scaleHeightPercent,
					imageDataHex,
					(imageDataHex.Length / 2) ) );
			}
		} // DoInsertImage

		// ----------------------------------------------------------------------
		protected override void DoEndDocument( IRtfInterpreterContext context )
		{
			if ( this.settings.Enabled && !string.IsNullOrEmpty( this.settings.EndDocumentText ) )
			{
				WriteLine( this.settings.EndDocumentText );
			}

			CloseStream();
		} // DoEndDocument

		// ----------------------------------------------------------------------
		private void WriteLine( string message )
		{
			if ( this.streamWriter == null )
			{
				return;
			}

			this.streamWriter.WriteLine( message );
			this.streamWriter.Flush();
		} // WriteLine

		// ----------------------------------------------------------------------
		private void EnsureDirectory()
		{
			FileInfo fi = new FileInfo( this.fileName );
			if ( !string.IsNullOrEmpty( fi.DirectoryName ) && !Directory.Exists( fi.DirectoryName ) )
			{
				Directory.CreateDirectory( fi.DirectoryName );
			}
		} // EnsureDirectory

		// ----------------------------------------------------------------------
		private void OpenStream()
		{
			if ( this.streamWriter != null )
			{
				return;
			}
			this.streamWriter = new StreamWriter( this.fileName );
		} // OpenStream

		// ----------------------------------------------------------------------
		private void CloseStream()
		{
			if ( this.streamWriter == null )
			{
				return;
			}
			this.streamWriter.Close();
			this.streamWriter.Dispose();
			this.streamWriter = null;
		} // OpenStream

		// ----------------------------------------------------------------------
		// members
		private readonly string fileName;
		private readonly RtfInterpreterLoggerSettings settings;
		private StreamWriter streamWriter;

	} // class RtfInterpreterListenerFileLogger

} // namespace Itenso.Rtf.Interpreter
// -- EOF -------------------------------------------------------------------
	