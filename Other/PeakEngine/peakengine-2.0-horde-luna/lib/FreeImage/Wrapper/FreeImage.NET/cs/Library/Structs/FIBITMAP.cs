// ==========================================================
// FreeImage 3 .NET wrapper
// Original FreeImage 3 functions and .NET compatible derived functions
//
// Design and implementation by
// - Jean-Philippe Goerke (jpgoerke@users.sourceforge.net)
// - Carsten Klein (cklein05@users.sourceforge.net)
//
// Contributors:
// - David Boland (davidboland@vodafone.ie)
//
// Main reference : MSDN Knowlede Base
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

// ==========================================================
// CVS
// $Revision: 1.2 $
// $Date: 2008/06/16 15:17:37 $
// $Id: FIBITMAP.cs,v 1.2 2008/06/16 15:17:37 cklein05 Exp $
// ==========================================================

using System;
using System.Runtime.InteropServices;

namespace FreeImageAPI
{
	/// <summary>
	/// The <b>FIBITMAP</b> structure is a handle to a FreeImage bimtap.
	/// </summary>
	/// <remarks>
	/// The handle represented by a <b>FIBITBAP</b> structure provides
	/// access to either a singlepage bitmap or exactly one page of
	/// a multipage bitmap.
	/// </remarks>
	[Serializable, StructLayout(LayoutKind.Sequential)]
	public struct FIBITMAP : IComparable, IComparable<FIBITMAP>, IEquatable<FIBITMAP>
	{
		private IntPtr data;

		/// <summary>
		/// Initializes a new instance of the <see cref="FIBITMAP"/> structure to the value indicated by
		/// a specified pointer to a native <see cref="FIBITMAP"/> structure.
		/// </summary>
		/// <param name="ptr">A pointer to a native <see cref="FIBITMAP"/> structure.</param>
		public FIBITMAP(int ptr)
		{
			data = new IntPtr(ptr);
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="FIBITMAP"/> structure to the value indicated by
		/// a specified pointer to a native <see cref="FIBITMAP"/> structure.
		/// </summary>
		/// <param name="ptr">A pointer to a native <see cref="FIBITMAP"/> structure.</param>
		public FIBITMAP(IntPtr ptr)
		{
			data = ptr;
		}

		/// <summary>
		/// Tests whether two specified <see cref="FIBITMAP"/> structures are equivalent.
		/// </summary>
		/// <param name="left">The <see cref="FIBITMAP"/> that is to the left of the equality operator.</param>
		/// <param name="right">The <see cref="FIBITMAP"/> that is to the right of the equality operator.</param>
		/// <returns>
		/// <b>true</b> if the two <see cref="FIBITMAP"/> structures are equal; otherwise, <b>false</b>.
		/// </returns>
		public static bool operator ==(FIBITMAP left, FIBITMAP right)
		{
			return (left.data == right.data);
		}

		/// <summary>
		/// Tests whether two specified <see cref="FIBITMAP"/> structures are different.
		/// </summary>
		/// <param name="left">The <see cref="FIBITMAP"/> that is to the left of the inequality operator.</param>
		/// <param name="right">The <see cref="FIBITMAP"/> that is to the right of the inequality operator.</param>
		/// <returns>
		/// <b>true</b> if the two <see cref="FIBITMAP"/> structures are different; otherwise, <b>false</b>.
		/// </returns>
		public static bool operator !=(FIBITMAP left, FIBITMAP right)
		{
			return (left.data != right.data);
		}

		/// <summary>
		/// Converts the pointer specified in <paramref name="ptr"/> to a <see cref="FIBITMAP"/> structure.
		/// </summary>
		/// <param name="ptr">A 32-bit value to be converted into a <see cref="FIBITMAP"/> structure.</param>
		/// <returns>A <see cref="FIBITMAP"/> structure initialized with the specified pointer.</returns>
		public static implicit operator FIBITMAP(int ptr)
		{
			return new FIBITMAP(ptr);
		}

		/// <summary>
		/// Converts the <see cref="FIBITMAP"/> structure specified in <paramref name="handle"/> to a 32-bit value.
		/// </summary>
		/// <param name="handle">A <see cref="FIBITMAP"/> structure to be converted into a 32-bit value.</param>
		/// <returns>A 32-bit value initialized with the pointer of the <see cref="FIBITMAP"/> structure.</returns>
		public static implicit operator int(FIBITMAP handle)
		{
			return handle.data.ToInt32();
		}

		/// <summary>
		/// Converts the pointer specified in <paramref name="ptr"/> to a <see cref="FIBITMAP"/> structure.
		/// </summary>
		/// <param name="ptr">A 32-bit value to be converted into a <see cref="FIBITMAP"/> structure.</param>
		/// <returns>A <see cref="FIBITMAP"/> structure initialized with the specified pointer.</returns>
		public static implicit operator FIBITMAP(IntPtr ptr)
		{
			return new FIBITMAP(ptr);
		}

		/// <summary>
		/// Converts the <see cref="FIBITMAP"/> structure specified in <paramref name="handle"/> to an IntPtr.
		/// </summary>
		/// <param name="handle">A <see cref="FIBITMAP"/> structure to be converted into an IntPtr.</param>
		/// <returns>An IntPtr initialized with the pointer of the <see cref="FIBITMAP"/> structure.</returns>
		public static implicit operator IntPtr(FIBITMAP handle)
		{
			return handle.data;
		}

		/// <summary>
		/// Gets whether the pointer is a null pointer or not.
		/// </summary>
		/// <value><b>true</b> if this <see cref="FIBITMAP"/> is a null pointer;
		/// otherwise, <b>false</b>.</value>		
		public bool IsNull
		{
			get
			{
				return (data == IntPtr.Zero);
			}
		}

		/// <summary>
		/// Converts the numeric value of the <see cref="FIBITMAP"/> object
		/// to its equivalent string representation.
		/// </summary>
		/// <returns>The string representation of the value of this instance.</returns>
		public override string ToString()
		{
			return data.ToString();
		}

		/// <summary>
		/// Returns a hash code for this <see cref="FIBITMAP"/> structure.
		/// </summary>
		/// <returns>An integer value that specifies the hash code for this <see cref="FIBITMAP"/>.</returns>
		public override int GetHashCode()
		{
			return data.GetHashCode();
		}

		/// <summary>
		/// Determines whether the specified <see cref="Object"/> is equal to the current <see cref="Object"/>.
		/// </summary>
		/// <param name="obj">The <see cref="Object"/> to compare with the current <see cref="Object"/>.</param>
		/// <returns><b>true</b> if the specified <see cref="Object"/> is equal to the current <see cref="Object"/>; otherwise, <b>false</b>.</returns>
		public override bool Equals(object obj)
		{
			return ((obj is FIBITMAP) && (this == ((FIBITMAP)obj)));
		}

		/// <summary>
		/// Indicates whether the current object is equal to another object of the same type.
		/// </summary>
		/// <param name="other">An object to compare with this object.</param>
		/// <returns><b>true</b> if the current object is equal to the other parameter; otherwise, <b>false</b>.</returns>
		public bool Equals(FIBITMAP other)
		{
			return (this == other);
		}

		/// <summary>
		/// Compares this instance with a specified <see cref="Object"/>.
		/// </summary>
		/// <param name="obj">An object to compare with this instance.</param>
		/// <returns>A 32-bit signed integer indicating the lexical relationship between the two comparands.</returns>
		/// <exception cref="ArgumentException"><paramref name="obj"/> is not a <see cref="FIBITMAP"/>.</exception>
		public int CompareTo(object obj)
		{
			if (obj == null)
			{
				return 1;
			}
			if (!(obj is FIBITMAP))
			{
				throw new ArgumentException();
			}
			return CompareTo((FIBITMAP)obj);
		}

		/// <summary>
		/// Compares this instance with a specified <see cref="FIBITMAP"/> object.
		/// </summary>
		/// <param name="other">A <see cref="FIBITMAP"/> to compare.</param>
		/// <returns>A signed number indicating the relative values of this instance
		/// and <paramref name="other"/>.</returns>
		public int CompareTo(FIBITMAP other)
		{
			return this.data.ToInt64().CompareTo(other.data.ToInt64());
		}
	}
}