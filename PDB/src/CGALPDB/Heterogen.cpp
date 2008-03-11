/* Copyright 2004
   Stanford University

   This file is part of the DSR PDB Library.

   The DSR PDB Library is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or (at your
   option) any later version.

   The DSR PDB Library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with the DSR PDB Library; see the file LICENSE.LGPL.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA. */

#include <CGAL/PDB/Heterogen.h>
#include <CGAL/PDB/internal/Error_logger.h>
#include <iostream>
#include <limits>

#include <sstream>
#include <cstdio>
CGAL_PDB_BEGIN_NAMESPACE


void Heterogen::copy_from(const Heterogen &o) {
  atoms_= o.atoms_;
  type_= o.type_;
  bonds_.clear();
  for (unsigned int i=0; i< o.bonds_.size(); ++i) {
    std::string nma=bonds_[i].first.key();
    std::string nmb=bonds_[i].second.key();
    Bond nb(Bond_endpoint(find(nma)),
            Bond_endpoint(find(nmb)));
    bonds_.push_back(nb);
  }
  chain_=o.chain_;
}


bool Heterogen::connect(Atom::Index a, Atom::Index b) {
  Atom_iterator ita=atoms_end(), itb= atoms_end();
  if (b < a) std::swap(a,b);
  for (Atom_iterator it= atoms_begin(); it != atoms_end(); ++it) {
    if (it->atom().index() == a) {
      ita= it;
    }
    if (it->atom().index() == b) {
      itb= it;
    }
  }
  if (ita == atoms_end()) return false;
  if (itb == atoms_end()) return false;
  for (unsigned int i=0; i< bonds_.size(); ++i) {
    if (bonds_[i].first.key() == ita->key() 
        && bonds_[i].second.key() == itb->key()){
      return true;
    }
  }
  bonds_.push_back(Bond(Bond_endpoint(ita),
                        Bond_endpoint(itb)));
  return true;
}

void Heterogen::swap_with(Heterogen &o) {
  std::swap(atoms_, o.atoms_);
  std::swap(bonds_, o.bonds_);
  std::swap(type_, o.type_);
  std::swap(chain_, o.chain_);
}




void Heterogen::dump(std::ostream &out) const {
  
}

int Heterogen::write(std::string name, int num, 
                     int start_index, std::ostream &out) const {
  char line[81];
 
  for (Atoms::const_iterator it= atoms_.begin(); it != atoms_.end(); ++it) {
    Atom_key al= it->key();
    //Point pt= res->cartesian_coords(al);
    const Atom &a= it->atom();
    a.set_index(Atom::Index(start_index));
    Point pt = a.point();
    //char chain=' ';

    //"HETATM%5d %4s %3s  %4d    %8.3f%8.3f%8.3f%6.2f%6.2f      %4s%2s%2s";
    std::sprintf(line, CGAL_PDB_INTERNAL_NS::hetatom_line_oformat_,
                 start_index++, al.c_str(),
                 name.c_str(), num,
                 pt.x(), pt.y(), pt.z(), a.occupancy(), a.temperature_factor(),
                 a.element().c_str(), "  ");
    out << line << std::endl;
    //++anum;
  }
  std::map<int, std::vector<int> > connects;
  for (unsigned int i=0; i< bonds_.size(); ++i) {
    int ia= bonds_[i].first.atom().index().index();
    int ib= bonds_[i].second.atom().index().index();
    connects[ia].push_back(ib);
    connects[ib].push_back(ia);
  }
  for (std::map<int, std::vector<int> >::const_iterator it= connects.begin();
       it != connects.end(); ++it) {
    out << "CONECT " << it->first;
    for (unsigned int i=0; i< it->second.size(); ++i) {
      out << " " << it->second[i];
    }
    out << std::endl;
  }
  return start_index;
}


 


Heterogen::Heterogen(std::string name): atoms_(20), type_(name){
}




CGAL_PDB_END_NAMESPACE
