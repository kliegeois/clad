//--------------------------------------------------------------------*- C++ -*-
// clad - the C++ Clang-based Automatic Differentiator
// version: $Id: ClangPlugin.cpp 7 2013-06-01 22:48:03Z v.g.vassilev@gmail.com $
// author:  Vassil Vassilev <vvasilev-at-cern.ch>
//------------------------------------------------------------------------------

#ifndef CLAD_KOKKOS_VIEW_ACCESS_VISITOR_H
#define CLAD_KOKKOS_VIEW_ACCESS_VISITOR_H

namespace clad {

  class KokkosViewAccessVisitor {
    public:
      KokkosViewAccessVisitor (){}

      void Visit(const clang::Stmt *Node, double record_view_names = false) {
        if (llvm::isa<clang::CallExpr>(Node)) {
          if (llvm::isa<clang::CXXOperatorCallExpr>(Node)) {
            auto OCE = llvm::dyn_cast<clang::CXXOperatorCallExpr>(Node);

            std::string constructedTypeName = OCE->getDirectCallee()->getQualifiedNameAsString();

            if(constructedTypeName.find("Kokkos::View") != std::string::npos && constructedTypeName.find("::operator()") != std::string::npos) {
              view_accesses.push_back(OCE);
            }
          }
          else {
            record_view_names = true;
          }
        }
        if (llvm::isa<clang::DeclRefExpr>(Node)) {
          auto DRE = llvm::dyn_cast<clang::DeclRefExpr>(Node);
          auto VD = llvm::dyn_cast<clang::VarDecl>(DRE->getDecl());

          if(VD && record_view_names) {
            std::string constructedTypeName = clang::QualType::getAsString(VD->getType().split(), clang::PrintingPolicy{ {} });
            if (utils::IsKokkosView(constructedTypeName)) {
              std::string name = DRE->getNameInfo().getName().getAsString();

              if(!std::count(view_names.begin(), view_names.end(), name)) {
                view_names.push_back(name);
              }
            }
            return;
          }
        }

        for (const clang::Stmt *SubStmt : Node->children())
          Visit(SubStmt, record_view_names);
      }

      std::vector<std::string> view_names;
      std::vector<const clang::CXXOperatorCallExpr*> view_accesses;
  };
} // end namespace clad


#endif // CLAD_KOKKOS_VIEW_ACCESS_VISITOR_H
