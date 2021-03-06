// -- FILE ------------------------------------------------------------------
// name       : RtfGroup.cs
// project    : RTF Framelet
// created    : Leon Poyyayil - 2008.05.19
// language   : c#
// environment: .NET 2.0
// copyright  : (c) 2004-2009 by Itenso GmbH, Switzerland
// --------------------------------------------------------------------------
using System;
using System.Text;
using Itenso.Sys;

namespace Itenso.Rtf.Model
{

	// ------------------------------------------------------------------------
	public sealed class RtfGroup : RtfElement, IRtfGroup
	{

		// ----------------------------------------------------------------------
		public RtfGroup()
			: base( RtfElementKind.Group )
		{
		} // RtfGroup

		// ----------------------------------------------------------------------
		public IRtfElementCollection Contents
		{
			get { return this.contents; }
		} // Contents

		// ----------------------------------------------------------------------
		public RtfElementCollection WritableContents
		{
			get { return this.contents; }
		} // WritableContents

		// ----------------------------------------------------------------------
		public string Destination
		{
			get
			{
				if ( this.contents.Count > 0 )
				{
					IRtfElement firstChild = this.contents[ 0 ];
					if ( firstChild.Kind == RtfElementKind.Tag )
					{
						IRtfTag firstTag = (IRtfTag)firstChild;
						if ( RtfSpec.TagExtensionDestination.Equals( firstTag.Name ) )
						{
							if ( this.contents.Count > 1 )
							{
								IRtfElement secondChild = this.contents[ 1 ];
								if ( secondChild.Kind == RtfElementKind.Tag )
								{
									IRtfTag secondTag = (IRtfTag)secondChild;
									return secondTag.Name;
								}
							}
						}
						return firstTag.Name;
					}
				}
				return null;
			}
		} // Destination

		// ----------------------------------------------------------------------
		public bool IsExtensionDestination
		{
			get
			{
				if ( this.contents.Count > 0 )
				{
					IRtfElement firstChild = this.contents[ 0 ];
					if ( firstChild.Kind == RtfElementKind.Tag )
					{
						IRtfTag firstTag = (IRtfTag)firstChild;
						if ( RtfSpec.TagExtensionDestination.Equals( firstTag.Name ) )
						{
							return true;
						}
					}
				}
				return false;
			}
		} // IsExtensionDestination

		// ----------------------------------------------------------------------
		public IRtfGroup SelectChildGroupWithDestination( string destination )
		{
			if ( destination == null )
			{
				throw new ArgumentNullException( "destination" );
			}
			foreach ( IRtfElement child in this.contents )
			{
				if ( child.Kind == RtfElementKind.Group )
				{
					IRtfGroup group = (IRtfGroup)child;
					if ( destination.Equals( group.Destination ) )
					{
						return group;
					}
				}
			}
			return null;
		} // SelectChildGroupWithDestination

		// ----------------------------------------------------------------------
		public override string ToString()
		{
			StringBuilder buf = new StringBuilder( "{" );
			int count = this.contents.Count;
			buf.Append( count );
			buf.Append( " items" );
			if ( count > 0 )
			{
				// visualize the first two child elements for convenience during debugging
				buf.Append( ": [" );
				buf.Append( this.contents[ 0 ] );
				if ( count > 1 )
				{
					buf.Append( ", " );
					buf.Append( this.contents[ 1 ] );
					if ( count > 2 )
					{
						buf.Append( ", " );
						if ( count > 3 )
						{
							buf.Append( "..., " );
						}
						buf.Append( this.contents[ count - 1 ] );
					}
				}
				buf.Append( "]" );
			}
			buf.Append( "}" );
			return buf.ToString();
		} // ToString

		// ----------------------------------------------------------------------
		protected override void DoVisit( IRtfElementVisitor visitor )
		{
			visitor.VisitGroup( this );
		} // DoVisit

		// ----------------------------------------------------------------------
		protected override bool IsEqual( object obj )
		{
			RtfGroup compare = obj as RtfGroup; // guaranteed to be non-null
			return compare != null && base.IsEqual( obj ) &&
				this.contents.Equals( compare.contents );
		} // IsEqual

		// ----------------------------------------------------------------------
		protected override int ComputeHashCode()
		{
			return HashTool.AddHashCode( base.ComputeHashCode(), this.contents );
		} // ComputeHashCode

		// ----------------------------------------------------------------------
		// members
		private readonly RtfElementCollection contents = new RtfElementCollection();

	} // class RtfGroup

} // namespace Itenso.Rtf.Model
// -- EOF -------------------------------------------------------------------
