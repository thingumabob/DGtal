/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/
//LICENSE-END
#pragma once

/**
 * @file HyperRectDomain_Iterator.h
 * @author David Coeurjolly (\c david.coeurjolly@liris.cnrs.fr )
 * @author Guillaume Damiand (\c guillaume.damiand@liris.cnrs.fr )
 * Laboratoire d'InfoRmatique en Image et Systèmes d'information - LIRIS (CNRS, UMR 5205), CNRS, France
 *
 * @date 2010/05/31
 *
 * Header file for module HyperRectDomain_Iterator.cpp
 *
 * This file is part of the DGtal library.
 */

#if defined(HyperRectDomain_Iterator_RECURSES)
#error Recursive header files inclusion detected in HyperRectDomain_Iterator.h
#else // defined(HyperRectDomain_Iterator_RECURSES)
/** Prevents recursive inclusion of headers. */
#define HyperRectDomain_Iterator_RECURSES

#if !defined HyperRectDomain_Iterator_h
/** Prevents repeated inclusion of headers. */
#define HyperRectDomain_Iterator_h

//////////////////////////////////////////////////////////////////////////////
// Inclusions
#include <iostream>
#include <vector>
#include "DGtal/base/Common.h"
//////////////////////////////////////////////////////////////////////////////

namespace DGtal
{

/////////////////////////////////////////////////////////////////////////////
// class HyperRectDomain_Iterator
  /**
   * Description of class 'HyperRectDomain_Iterator' <p>
   * Aim:
   */
  template<typename TPoint, typename TSize>
  class HyperRectDomain_Iterator
  {
    public:
      typedef std::bidirectional_iterator_tag iterator_category; ///\todo construct a RANDOM-ACCESS iterator
      typedef TPoint value_type;
      typedef ptrdiff_t difference_type;
      typedef TPoint* pointer;
      typedef TPoint& reference;
			typedef TSize Size;


      HyperRectDomain_Iterator( const TPoint & p, const TPoint& lower, const TPoint &upper )
          : myPoint( p ), mylower( lower ), myupper( upper ),  myCurrentPos( 0 ),
          myUseSubDomain( false )
      {
        ASSERT( lower <= upper );
        ASSERT( lower <= p && p <= upper );
      }

#ifdef CPP0X_INITIALIZER_LIST
      HyperRectDomain_Iterator(const TPoint & p, const TPoint& lower,
          const TPoint &upper,
          std::initializer_list<Size> subDomain)
          : myPoint( p ), mylower( lower ), myupper( upper ),  myCurrentPos( 0 ),
          myUseSubDomain( true )
      {
        ASSERT( lower <= upper );
        ASSERT( lower <= p && p <= upper );
        ASSERT( subDomain.size() <= TPoint::Dimension );
        mySubDomain.reserve( subDomain.size() );
        for ( const unsigned int *c = subDomain.begin();
            c != subDomain.end(); ++c )
        {
          ASSERT( *c <= TPoint::Dimension );
          mySubDomain.push_back( *c );
        }

        // TODO: check the validity of the subDomain ?
      }
#endif
      HyperRectDomain_Iterator(const TPoint & p, const TPoint& lower,
          const TPoint &upper,
          const std::vector<Size> &subDomain)
          : myPoint( p ), mylower( lower ), myupper( upper ),  myCurrentPos( 0 ),
          myUseSubDomain( true )
      {
        ASSERT( lower <= upper );
        ASSERT( lower <= p && p <= upper );
        ASSERT( subDomain.size() <= TPoint::Dimension );
        mySubDomain.reserve( subDomain.size() );
        for ( std::vector<Size>::const_iterator it = subDomain.begin();
            it != subDomain.end(); ++it )
        {
          ASSERT( *it <= TPoint::Dimension );
          mySubDomain.push_back( *it );
        }

        // TODO: check the validity of the subDomain ?
      }


      const TPoint & operator*() const
      {
        // pb for reverse iterator ASSERT(mylower<=myPoint && myPoint<=myupper); // we must be between [begin,end]
        return myPoint;
      }

      /**
      * Operator ==
      *
      */
      bool operator== ( const HyperRectDomain_Iterator<TPoint,TSize> &it ) const
      {
        return ( myPoint == ( *it ) );
      }

      /**
      * Operator !=
      *
      */
      bool operator!= ( const HyperRectDomain_Iterator<TPoint,TSize> &aIt ) const
      {
        return ( myPoint != ( *aIt ) );
      }

      /**
      * Implements the next() method to scan the domain points dimension by dimension
      * (lexicographic order).
      **/
      void nextLexicographicOrder()
      {
        myPoint.at( myCurrentPos ) ++;
        if (( myCurrentPos < TPoint::Dimension - 1 ) &&
            ( myPoint.at( myCurrentPos )  >  myupper.at( myCurrentPos ) ) )
        {
          do
          {
            myPoint.at( myCurrentPos ) = mylower.at( myCurrentPos );
            myCurrentPos++;
            if ( myCurrentPos < TPoint::Dimension )
              myPoint.at( myCurrentPos ) ++;
          }
          while (( myCurrentPos < TPoint::Dimension - 1 ) &&
              ( myPoint.at( myCurrentPos )  >  myupper.at( myCurrentPos ) ) );
          myCurrentPos = 0;
        }
      }

      /**
      * Implements the next() method to scan the domain points dimension by dimension
      * (by using the subDomain order given by the user).
      **/
      void nextSubDomainOrder()
      {
        ASSERT( myCurrentPos < mySubDomain.size() );
        myPoint.at( mySubDomain[myCurrentPos] ) ++;

        if ( myCurrentPos < mySubDomain.size() - 1 &&
            myPoint.at( mySubDomain[myCurrentPos] ) >
            myupper.at( mySubDomain[myCurrentPos] ) )
        {
          do
          {
            myPoint.at( mySubDomain[myCurrentPos] ) =
              mylower.at( mySubDomain[myCurrentPos] );
            myCurrentPos++;
            if ( myCurrentPos < mySubDomain.size() )
              myPoint.at( mySubDomain[myCurrentPos] ) ++;
          }
          while (( myCurrentPos < mySubDomain.size() - 1  ) &&
              ( myPoint.at( mySubDomain[myCurrentPos] )  >
                  myupper.at( mySubDomain[myCurrentPos] ) ) );
          myCurrentPos = 0;
        }
      }

      /**
      * Operator ++ (++it)
      */
      HyperRectDomain_Iterator<TPoint,TSize> &operator++()
      {
        if ( myUseSubDomain )
          nextSubDomainOrder();
        else
          nextLexicographicOrder();
        return *this;
      }

      /**
      * Operator ++ (it++)
      *
      */
      HyperRectDomain_Iterator<TPoint,TSize> operator++ ( int )
      {
        HyperRectDomain_Iterator<TPoint,TSize> tmp = *this;
        ++*this;
        return tmp;
      }

      /**
      * Implements the prev() method to scan the domain points dimension by dimension
      * (lexicographic order).
      **/
      void prevLexicographicOrder()
      {
        myPoint.at( myCurrentPos ) --;
        if (( myCurrentPos < TPoint::Dimension - 1 ) &&
            ( myPoint.at( myCurrentPos )  <  mylower.at( myCurrentPos ) ) )
        {
          do
          {
            myPoint.at( myCurrentPos ) = myupper.at( myCurrentPos );
            myCurrentPos++;
            if ( myCurrentPos < TPoint::Dimension )
              myPoint.at( myCurrentPos ) --;
          }
          while (( myCurrentPos < TPoint::Dimension - 1 ) &&
              ( myPoint.at( myCurrentPos )  <  mylower.at( myCurrentPos ) ) );
          myCurrentPos = 0;
        }
      }

      /**
       * Implements the prev() method to scan the domain points dimension by dimension
       * (subDomain order).
       **/
      void prevSubDomainOrder()
      {
        ASSERT( myCurrentPos < mySubDomain.size() );
        myPoint.at( mySubDomain[myCurrentPos] ) --;

        if (  myCurrentPos < mySubDomain.size() - 1 &&
            myPoint.at( mySubDomain[myCurrentPos] )  <
            mylower.at( mySubDomain[myCurrentPos] ) )
        {
          do
          {
            myPoint.at( mySubDomain[myCurrentPos] ) =
              myupper.at( mySubDomain[myCurrentPos] );
            myCurrentPos++;
            if ( myCurrentPos < mySubDomain.size() )
              myPoint.at( mySubDomain[myCurrentPos] ) --;
          }
          while (( myCurrentPos < mySubDomain.size() - 1 ) &&
              ( myPoint.at( mySubDomain[myCurrentPos] )  <
                  mylower.at( mySubDomain[myCurrentPos] ) ) );
          myCurrentPos = 0;
        }
      }

      /**
      * Operator ++ (++it)
      *
      */
      HyperRectDomain_Iterator<TPoint,TSize> &operator--()
      {
        if ( myUseSubDomain )
          prevSubDomainOrder();
        else
          prevLexicographicOrder();
        return *this;
      }

      /**
      * Operator ++ (it++)
      */
      HyperRectDomain_Iterator<TPoint,TSize> &operator-- ( int )
      {
        HyperRectDomain_Iterator<TPoint,TSize> tmp = *this;
        --*this;
        return tmp;
      }


    private:
      ///Current Point in the domain
      TPoint myPoint;
      ///Copies of the Domain limits
      TPoint mylower, myupper;
      ///Second index of the iterator position
      Size myCurrentPos;
      ///Vector of subDomain on dimension, to fix the order in which dimensions
      /// are considered.
      std::vector<Size> mySubDomain;
      /// True iff we use the vector of subDomain, otherwise dimensions are
      /// considered in increasing order.
      bool myUseSubDomain;
  };


} //namespace
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#endif // !defined HyperRectDomain_Iterator_h

#undef HyperRectDomain_Iterator_RECURSES
#endif // else defined(HyperRectDomain_Iterator_RECURSES)
