/**
 * Copyright (c) 2018 Melown Technologies SE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * *  Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <ogr_core.h>

#include <sstream>
#include <string>
#include <vector>
#include <mutex>

#include <boost/python.hpp>
#include <boost/python/scope.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/handle.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <stdint.h>

#include "dbglog/dbglog.hpp"

#include "pysupport/package.hpp"
#include "pysupport/class.hpp"
#include "pysupport/enum.hpp"
#include "pysupport/converters.hpp"

#include "../mesh.hpp"
#include "../meshop.hpp"

namespace fs = boost::filesystem;
namespace bp = boost::python;
namespace bpc = boost::python::converter;

namespace geometry { namespace py {

typedef geometry::Face::index_type index_type;

void addFace3(Mesh &m, index_type a, index_type b, index_type c)
{
    return m.addFace(a, b, c);
}

void addFace6(Mesh &m, index_type a, index_type b, index_type c, index_type ta
              , index_type tb, index_type tc)
{
    return m.addFace(a, b, c, ta, tb, tc);
}

void addFace7(Mesh &m, index_type a, index_type b, index_type c, index_type ta
              , index_type tb, index_type tc, unsigned int imageId)
{
    return m.addFace(a, b, c, ta, tb, tc, imageId);
}

Mesh clip2(const Mesh &m, const math::Extents2 &e)
{
    return *geometry::clip(m, e);
}

Mesh clip3(const Mesh &m, const math::Extents3 &e)
{
    return *geometry::clip(m, e);
}

void appendMesh(Mesh &mesh, const Mesh &added)
{
    return geometry::append(mesh, added);
}

} } // namespace geometry::py

BOOST_PYTHON_MODULE(melown_geometry)
{
    using namespace bp;
    namespace py = geometry::py;

    typedef geometry::Face::index_type index_type;

    const return_internal_reference<> InternalRef;

    auto Face = class_<geometry::Face>("Face", init<const geometry::Face&>())
        .def(init<>())
        .def(init<index_type, index_type, index_type>())
        .def(init<index_type, index_type, index_type
             , index_type, index_type, index_type>())
        .def(init<index_type, index_type, index_type
             , index_type, index_type, index_type, unsigned int>())

        .def("normal", &geometry::Face::normal)
        .def("clear", &geometry::Face::clear)
        .def("degenerate", &geometry::Face::degenerate)

        .def_readwrite("imageId", &geometry::Face::imageId)
        .def_readwrite("a", &geometry::Face::a)
        .def_readwrite("b", &geometry::Face::b)
        .def_readwrite("c", &geometry::Face::c)
        .def_readwrite("ta", &geometry::Face::ta)
        .def_readwrite("tb", &geometry::Face::tb)
        .def_readwrite("tc", &geometry::Face::tc)
        ;


    {
        // wrap vector of Face
        bp::scope scope(Face);
        class_<geometry::Face::list>("list")
            .def(vector_indexing_suite<geometry::Face::list>())
            ;
    }

    auto Mesh = class_<geometry::Mesh>
        ("Mesh", init<const geometry::Mesh&>())
        .def(init<>())
        .def("normal", &geometry::Mesh::normal)
        .def("addFace", &py::addFace3)
        .def("addFace", &py::addFace6)
        .def("addFace", &py::addFace7)
        .def("a", &geometry::Mesh::a, InternalRef)
        .def("b", &geometry::Mesh::b, InternalRef)
        .def("c", &geometry::Mesh::c, InternalRef)
        .def("ta", &geometry::Mesh::ta, InternalRef)
        .def("tb", &geometry::Mesh::tb, InternalRef)
        .def("tc", &geometry::Mesh::tc, InternalRef)
        .def("degenerate", &geometry::Mesh::degenerate)
        .def("good", &geometry::Mesh::good)
        .def("sortFacesByImageId", &geometry::Mesh::sortFacesByImageId)
        .def("area", &geometry::Mesh::area)
        .def("txArea", &geometry::Mesh::txArea)
        .def("barycenter", &geometry::Mesh::barycenter)
        ;

    pysupport::def_readwrite(Mesh, "vertices", &geometry::Mesh::vertices);
    pysupport::def_readwrite(Mesh, "tCoords", &geometry::Mesh::tCoords);
    pysupport::def_readwrite(Mesh, "faces", &geometry::Mesh::faces);

    def<geometry::Mesh (*)(const fs::path&)>("loadPly", &geometry::loadPly);
    def<geometry::Mesh (*)(const fs::path&)>("loadObj", &geometry::loadObj);
    def<void (*)(const geometry::Mesh&, const fs::path&)>
        ("savePly", &geometry::saveAsPly);
    def<void (*)(const geometry::Mesh&, const fs::path&, const std::string&)>
        ("saveObj", &geometry::saveAsObj);

    def("clip", &py::clip2);
    def("clip", &py::clip3);
    def("append", &py::appendMesh);
}

namespace geometry { namespace py {
PYSUPPORT_MODULE_IMPORT(geometry)
} } // namespace geometry::py
