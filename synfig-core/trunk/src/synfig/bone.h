/* === S Y N F I G ========================================================= */
/*!	\file bone.h
**	\brief Bone Header
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

/* === S T A R T =========================================================== */

#ifndef __SYNFIG_BONE_H
#define __SYNFIG_BONE_H

/* === H E A D E R S ======================================================= */

#include "matrix.h"
#include "uniqueid.h"
#include <vector>

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */
namespace synfig {

class Bone: public UniqueID
{
	/*
 --	** -- T Y P E S -----------------------------------------------------------
	*/

public:
	// typedef etl::handle<Bone> Handle;
	// typedef etl::loose_handle<Bone> LooseHandle;

	/*
 --	** -- D A T A -------------------------------------------------------------
	*/

private:
	//!This is the current origin of the bone relative to parent
	Point origin_;
	//!This is the origin of the bone at the setup time
	Point origin0_;
	//!This is the current angle if the bone relative to parent.
	Angle angle_;
	//!This is the angle of the bone at the setup time
	Angle angle0_;
	//!This is the current scale of the bone.
	Real scale_;
//	Real scale0_; // Scale0 is always = 1.0
	//!This is the length at setup time
	Real length_;
	//!This is the strength at setup time
	Real strength_;
	//!The parent bone.
	Bone *parent_;
	//!The geanological tree of parents
	std::vector<Bone *> parent_tree_;
public:
	//!Default constructor
	Bone();
	//!Constructor by origin and tip
	Bone(const Point &origin, const Point &tip);
	//!Construtor by origin, legth and parent (default no parent)
	Bone(const Point &origin, const Angle &angle, const Real &length, const Real &strength, Bone* p=0);

	//!Wrappers for origin_ & origin0_
	const Point& get_origin() {return origin_;}
	void set_origin(const Point &x) {origin_=x;}
	const Point& get_origin0() {return origin0_;}
	void set_origin0(const Point &x) {origin0_=x;}

	//!Wrappers for angle_ & angle0_
	const Angle& get_angle() {return angle_;}
	void set_angle(const Angle &x) {angle_=x;}
	const Angle& get_angle0() {return angle0_;}
	void set_angle0(const Angle &x) {angle0_=x;}

	//!Wrapper for scale
	const Real& get_scale() {return scale_;}
	void set_scale(const Real &x) {scale_=x;}

	//!Wrapper for length. Notice that a length of 0 is not allowed.
	const Real& get_length() {return length_;}
	void set_length(const Real &x) {length_=x<0.00001?0.00001:x;}

	//!Wrapper for strength
	const Real& get_strength() {return strength_;}
	void set_strength(const Real &x) {strength_=x;}

	//!This gets the calculated tip of the bone based on
	//!tip=origin+[length,0]*Rotate(alpha)*Scalex(scale)
	Point get_tip();

	//!Wrapper for parent bone
	const Bone &get_parent() {return *parent_;}
	void set_parent(Bone &p) {parent_=&(p);}

	//!Setup Transformation matrix.
	//!This matrix applied to a setup point in global
	//!coordinates calculates the local coordinates of
	//!the point relative to the current bone.
	Matrix get_setup_matrix();

	//!Animated Transformation matrix.
	//!This matrix applied to a setup point in local
	//!coordinates (the one obtained form the Setup
	//!Transformation matrix) would obtain the
	//!animated position of the point due the current
	//!bone influence
	Matrix get_animated_matrix();

	//!Clear genealogical tree of parents
	void clear_parent_tree() {parent_tree_.clear();}
}; // END of class Bone

}; // END of namespace synfig
/* === E N D =============================================================== */

#endif
/*
 * Alternative to Bone *parent_
 * ======================================================================
 * I think that we can leave the bone as a simple information holder
 * and only give it the responsability of:
 * Set and get:
 * 		-origin, origin0,
 * 		-angle, angle0,
 * 		-scale,
 * 		-lenght
 * 		-strength,
 * 		-ParentID: this is new: This is the UniqueID value of the parent bone.
 * 		Initially it is set to a non valid number (I think that -1 is fine)
 * 		so it means that it is a root bone. Later an external object can set it
 * 		to a valid UniqueID to mean that that's the parnent ID.
 * 		parent_tree is not needed.
 * 		-Skeletons Pointer (see below)
 * Also the bone should:
 * 		-get_setup_matrix
 * 		-get_animated_matrix
 * 		-get_tip
 *
 * Then it comes the concept of valuenode Skeletons. The Skeletons (notice that
 * it is plural because there can be more than one root bone) is like the Valuenode_Bline,
 * a linkable value.
 * It is like a normal list of bones (like bline is a normal list of blinepoints).
 * This list of bones has just that, bones. So the skeleton is not a expandble tree with
 * a potential loop problem; it is just a list of objects.
 *
 * The valuenode Skeletons is responsible of:
 * 1) Calculate the complete setup matrix of a bone based on the hierarchy
 * 2) Calculate the complete animated matrix of a bone base on the hierarchy
 * 3) (Re)Parent a bone. Or (Un)Parent it
 * 4) Remove the bone from the list. It would set the parent UniqueID=-1 and the Skeletons
 * pointer to be 0.
 * 5) Add a new bone to the list. The bone constructor would receive a Skeleton pointer and
 *   eventually a parent UniqueID besides the rest of information to fill the date (origin, etc.).
 * It would look like that:
 *
 * Valuenode Skeletons
 * 		Bone1 Valuenode Bone
 * 		Bone2 Valuenode Bone
 * 		...
 * 		BoneN Valuenode Bone
 *
 * To perform the tasks 1), 2), 3) or 4) the Skeletons Valuenode should perform a seek into the
 * list of bones by its UniqueID value. For example to calcualte the setup matrix it should
 * recostruct the bone hierarchy from the current bone to the root parent. Due to that now,
 * it is only stored the UniqueID of the parent (and not a pointer), it is the skeletons veluenode
 *  who have to perform all the job: find all the parents and multiply in the correct order its
 * matrixes. The same happen for the animated matrix.
 * For reparent it is the same. It is just a modification of the parent UniqueID.
 * Remove a bone from the list would imply remove all its children form the list. A warning should be triggered.
 * A bone that has a null pointer to Skeletons means that it is orphan completely. Its parent UniqueID
 * must be -1 in that case. Anyway the bone like that can be used again in other skeleton. Just need to
 * insert it in the Skeletons list by modifying the Skeletons pointer and filling the proper parent UniqueID.
 * The Skeletons pointer is not an animatable valuenode. It can be a Handle if you like. The parent
 * UniqueID can be animatable.
 * In this way every computation is slower but would be easier to define, visible to the user
 * and more consistent with the valuenode concept.
 *
 * This variation of concept doens't implies anything new in the VertexBone valuenode.
 * So the VertexBone Valuenode should look like:
 * VertexBone Valuenode
 * 		Vertex Free
 * 		Vertex Setup
 * 		Bone_weight_pairs List Valuenode
 * 			BoneWeightPair
 * 				Bone Valuenode
 * 				Weight Real
 *
 * AS well as the Bone has a pointer to the Skeletons it is possible to the Valunode VertexBone
 * to calculate the weighted matrixes as stated in the wiki. It jsut have to retrieve the
 * Skeleton Valuenode and ask it to perform the known tasks. Later the VertexBone valuenode
 * would do the weight calculation.
 *
 * How does it look?
 */
