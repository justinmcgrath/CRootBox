#ifndef PY_ROOTBOX_H_
#define PY_ROOTBOX_H_


/**
 *  A Python module for CRootbox based on boost.python
 *
 *  build shared library
 *  1. export LD_LIBRARY_PATH=~/boost_1_62_0/stage/lib
 *  2. g++ -std=c++11 -O3 -fpic -shared -o py_rootbox.so -Wl,-soname,"py_rootbox.so" PythonRootSystem.cpp -I/usr/include/python3.5 -L/home/daniel/boost_1_62_0/stage/lib -lboost_python Debug/ModelParameter.o Debug/Root.o Debug/RootSystem.o Debug/analysis.o Debug/sdf.o Debug/tropism.o
 *
 * sdf.h 		writePVPScript is not exposed, use RootSystem::write to write the geometry script
 * mymath.h		currently only Vector3d is exposed (lets see if we will need anything else)
 *
 */
//#define PYTHON_WRAPPER // UNCOMMENT TO BUILD SHARED LIBRARY

#ifdef PYTHON_WRAPPER

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "mymath.h"
#include "sdf.h"
#include "RootSystem.h"
#include "analysis.h"

using namespace boost::python;

/*
 * Functions overloading (by hand, there are also macros available)
 *
 * Tutorial example:
 * bool    (X::*fx1)(int)              = &X::f;
 * bool    (X::*fx2)(int, double)      = &X::f;
 * bool    (X::*fx3)(int, double, char)= &X::f;
 * int     (X::*fx4)(int, int, int)    = &X::f;
 *
 */
Vector3d (Vector3d::*times1)(const double) const = &Vector3d::times;
double (Vector3d::*times2)(const Vector3d&) const = &Vector3d::times;

void (Matrix3d::*times3)(const Matrix3d&) = &Matrix3d::times;
Vector3d (Matrix3d::*times4)(const Vector3d&) const = &Matrix3d::times;

std::string (SignedDistanceFunction::*writePVPScript)() const = &SignedDistanceFunction::writePVPScript;

double (AnalysisSDF::*getSummed1)(int st) const = &AnalysisSDF::getSummed;
double (AnalysisSDF::*getSummed2)(int st, SignedDistanceFunction* geometry) const = &AnalysisSDF::getSummed;



/**
 * Virtual functions (not sure if needed, or only if we derive classes from it in pyhton?), not working...
 *
 * it seems a bit tricky to make polymorphism work, we have to wrap the classes
 *
 * Tutorial example:
 * struct BaseWrap : Base, wrapper<Base>
 * {
 *    int f()
 *    {
 *        if (override f = this->get_override("f"))
 *            return f(); // *note*
 *        return Base::f();
 *    }
 *
 *    int default_f() { return this->Base::f(); }
 * };
 */
//struct SignedDistanceFunction_Wrap : SignedDistanceFunction, wrapper<SignedDistanceFunction>
//{
//	double getDist(const Vector3d& v) const {
//		if (override getDist = this->get_override("getDist"))
//			return getDist(v);
//		return SignedDistanceFunction::getDist(v);
//	}
//	double default_getDist(const Vector3d& v) { return this->SignedDistanceFunction::getDist(v); }
//};
//	class_<SignedDistanceFunction_Wrap, boost::noncopyable>("SignedDistanceFunction")
//	    .def("getDist", &SignedDistanceFunction_Wrap::getDist, &SignedDistanceFunction_Wrap::default_getDist)
//	; // TODO how does polymorphism work... (everything works fine, dont ask why)



/**
 * Expose classes to Pyhton module
 */
BOOST_PYTHON_MODULE(py_rootbox)
{
    /*
     * general
     */
    class_<std::vector<double>>("std_vector_double_")
        .def(vector_indexing_suite<std::vector<double>>() )
	;
    class_<std::vector<int>>("std_vector_int_")
        .def(vector_indexing_suite<std::vector<int>>() )
	;
	/*
	 * mymath.h
	 */
	class_<Vector3d>("Vector3d", init<>())
			.def(init<double,double,double>())
			.def(init<Vector3d&>())
			.def_readwrite("x",&Vector3d::x)
			.def_readwrite("y",&Vector3d::y)
			.def_readwrite("z",&Vector3d::z)
			.def("normalize",&Vector3d::normalize)
			.def("times",times1)
			.def("times",times2)
			.def("length",&Vector3d::length)
			.def("plus",&Vector3d::plus)
			.def("minus",&Vector3d::minus)
			.def("cross",&Vector3d::cross)
			.def("__str__",&Vector3d::toString)
			.def("__rep__",&Vector3d::toString)
	;
    class_<std::vector<Vector3d>>("std_vector_Vector3d_")
        .def(vector_indexing_suite<std::vector<Vector3d>>() )
	;
	class_<Matrix3d>("Matrix3d", init<>())
			.def(init<double,double,double,double,double,double,double,double,double>())
			.def(init<Vector3d&, Vector3d&, Vector3d&>())
			.def(init<Matrix3d&>())
			.def_readwrite("r0",&Matrix3d::r0)
			.def_readwrite("r1",&Matrix3d::r1)
			.def_readwrite("r2",&Matrix3d::r2)
			.def("rotX",&Matrix3d::rotX)
			.def("rotY",&Matrix3d::rotY)
			.def("rotZ",&Matrix3d::rotZ)
			.def("ons",&Matrix3d::ons)
			.def("det",&Matrix3d::det)
			.def("inverse",&Matrix3d::inverse)
			.def("column",&Matrix3d::column)
			.def("row",&Matrix3d::row)
			.def("times",times3)
			.def("times",times4)
			.def("__str__",&Matrix3d::toString)
			.def("__rep__",&Matrix3d::toString)
	;
	/*
	 * sdf.h
	 */
	class_<SignedDistanceFunction>("SignedDistanceFunction")
			.def("getDist",&SignedDistanceFunction::getDist)
			.def("writePVPScript", writePVPScript)
			.def("__str__",&SignedDistanceFunction::toString)
	;
    class_<std::vector<SignedDistanceFunction*>>("std_vector_SDF_")
        .def(vector_indexing_suite<std::vector<SignedDistanceFunction*>>() )
	;
	class_<SDF_PlantBox, bases<SignedDistanceFunction>>("SDF_PlantBox",init<double,double,double>())
			.def("getDist",&SDF_PlantBox::getDist)
                        .def("__str__",&SDF_PlantBox::toString)
	;
	class_<SDF_PlantContainer, bases<SignedDistanceFunction>>("SDF_PlantContainer",init<>())
			.def(init<double,double,double,double>())
			.def("getDist",&SDF_PlantContainer::getDist)
                        .def("__str__",&SDF_PlantContainer::toString)
	;
	class_<SDF_RotateTranslate, bases<SignedDistanceFunction>>("SDF_RotateTranslate",init<SignedDistanceFunction*,double,int,Vector3d&>())
			.def(init<SignedDistanceFunction*,Vector3d&>())
			.def("getDist",&SDF_RotateTranslate::getDist)
                        .def("__str__",&SDF_RotateTranslate::toString)
	;
	enum_<SDF_RotateTranslate::SDF_Axes>("SDF_Axis")
	    .value("xaxis", SDF_RotateTranslate::SDF_Axes::xaxis)
	    .value("yaxis", SDF_RotateTranslate::SDF_Axes::yaxis)
	    .value("zaxis", SDF_RotateTranslate::SDF_Axes::zaxis)
	;
	class_<SDF_Intersection, bases<SignedDistanceFunction>>("SDF_Intersection",init<std::vector<SignedDistanceFunction*>>())
			.def(init<SignedDistanceFunction*,SignedDistanceFunction*>())
			.def("getDist",&SDF_Intersection::getDist)
			.def("__str__",&SDF_Intersection::toString)
	;
	class_<SDF_Union, bases<SDF_Intersection>>("SDF_Union",init<std::vector<SignedDistanceFunction*>>())
			.def(init<SignedDistanceFunction*,SignedDistanceFunction*>())
			.def("getDist",&SDF_Union::getDist)
			.def("__str__",&SDF_Union::toString)
	;
	class_<SDF_Difference, bases<SDF_Intersection>>("SDF_Difference",init<std::vector<SignedDistanceFunction*>>())
			.def(init<SignedDistanceFunction*,SignedDistanceFunction*>())
			.def("getDist",&SDF_Difference::getDist)
			.def("__str__",&SDF_Difference::toString)
	;
	class_<SDF_Complement, bases<SignedDistanceFunction>>("SDF_Complement",init<SignedDistanceFunction*>())
			.def("getDist",&SDF_Complement::getDist)
			.def("__str__",&SDF_Complement::toString)
	;
	class_<SDF_HalfPlane, bases<SignedDistanceFunction>>("SDF_HalfPlane",init<Vector3d&,Vector3d&>())
			.def(init<Vector3d&,Vector3d&,Vector3d&>())
			.def("getDist",&SDF_HalfPlane::getDist)
			.def_readwrite("o", &SDF_HalfPlane::o)
			.def_readwrite("n", &SDF_HalfPlane::n)
			.def_readwrite("p1", &SDF_HalfPlane::p1)
			.def_readwrite("p2", &SDF_HalfPlane::p2)
			.def("__str__",&SDF_HalfPlane::toString)
	;
	/*
	 * soil.h
	 */
	class_<SoilProperty>("SoilProperty",init<>())
			.def("getRelativeValue",&SoilProperty::getRelativeValue)
			.def("getAbsoluteValue",&SoilProperty::getAbsoluteValue)
	;
	class_<SoilPropertySDF>("SoilPropertySDF",init<>())
			.def(init<SignedDistanceFunction*, double, double, double>())
			.def_readwrite("sdf", &SoilPropertySDF::sdf)
			.def_readwrite("fmax", &SoilPropertySDF::fmax)
			.def_readwrite("fmin", &SoilPropertySDF::fmin)
			.def_readwrite("slope", &SoilPropertySDF::slope)
	;
	/*
	 * ModelParameter
	 */
	class_<RootTypeParameter>("RootTypeParameter", init<>())
			.def(init<RootTypeParameter&>())
			.def("realize",&RootTypeParameter::realize)
			.def("getLateralType",&RootTypeParameter::getLateralType)
			.def("getK",&RootTypeParameter::getK)
			.def_readwrite("type", &RootTypeParameter::type)
			.def_readwrite("lb", &RootTypeParameter::lb)
			.def_readwrite("lbs", &RootTypeParameter::lbs)
			.def_readwrite("la", &RootTypeParameter::la)
			.def_readwrite("las", &RootTypeParameter::las)
			.def_readwrite("ln", &RootTypeParameter::ln)
			.def_readwrite("lns", &RootTypeParameter::lns)
			.def_readwrite("nob", &RootTypeParameter::nob)
			.def_readwrite("nobs", &RootTypeParameter::nobs)
			.def_readwrite("r", &RootTypeParameter::r)
			.def_readwrite("rs", &RootTypeParameter::rs)
			.def_readwrite("a", &RootTypeParameter::a)
			.def_readwrite("as", &RootTypeParameter::as)
			.def_readwrite("colorR", &RootTypeParameter::colorR)
			.def_readwrite("colorG", &RootTypeParameter::colorG)
			.def_readwrite("colorB", &RootTypeParameter::colorB)
			.def_readwrite("tropismT", &RootTypeParameter::tropismT)
			.def_readwrite("tropismN", &RootTypeParameter::tropismN)
			.def_readwrite("tropismS", &RootTypeParameter::tropismS)
			.def_readwrite("dx", &RootTypeParameter::dx)
			.def_readwrite("theta", &RootTypeParameter::theta)
			.def_readwrite("thetas", &RootTypeParameter::thetas)
			.def_readwrite("rlt", &RootTypeParameter::rlt)
			.def_readwrite("rlts", &RootTypeParameter::rlts)
			.def_readwrite("gf", &RootTypeParameter::gf)
			.def_readwrite("name", &RootTypeParameter::name)
			.def_readwrite("successor", &RootTypeParameter::successor)
			.def_readwrite("successorP", &RootTypeParameter::successorP)
			.def_readwrite("sef", &RootTypeParameter::sef)
			.def_readwrite("sbpf", &RootTypeParameter::sbpf)
			.def_readwrite("saf", &RootTypeParameter::saf)
			.def("__str__",&RootTypeParameter::toString)
	;
	class_<RootParameter>("RootParameter", init<>())
			.def(init<int , double, double, const std::vector<double>&, int, double, double, double, double>())
			.def("set",&RootParameter::set)
			.def_readwrite("type", &RootParameter::type)
			.def_readwrite("lb", &RootParameter::lb)
			.def_readwrite("la", &RootParameter::la)
			.def_readwrite("ln", &RootParameter::ln)
			.def_readwrite("nob", &RootParameter::nob)
			.def_readwrite("r", &RootParameter::r)
			.def_readwrite("a", &RootParameter::a)
			.def_readwrite("theta", &RootParameter::theta)
			.def_readwrite("rlt", &RootParameter::rlt)
			.def("getK",&RootParameter::toString)
			.def("__str__",&RootParameter::toString)
	;
	class_<RootSystemParameter>("RootSystemParameter", init<>())
			.def("set",&RootSystemParameter::set)
			.def_readwrite("seedPos", &RootSystemParameter::seedPos)
			.def_readwrite("firstB", &RootSystemParameter::firstB)
			.def_readwrite("delayB", &RootSystemParameter::delayB)
			.def_readwrite("maxB", &RootSystemParameter::maxB)
			.def_readwrite("nC", &RootSystemParameter::nC)
			.def_readwrite("firstSB", &RootSystemParameter::firstSB)
			.def_readwrite("delaySB", &RootSystemParameter::delaySB)
			.def_readwrite("delayRC", &RootSystemParameter::delayRC)
			.def_readwrite("nz", &RootSystemParameter::nz)
			.def("__str__",&RootSystemParameter::toString)
	;
	/*
	 * RootSystem.h
	 */
        class_<RootSystem>("RootSystem")
		.def("setRootTypeParameter", &RootSystem::setRootTypeParameter)
		.def("getRootTypeParameter", &RootSystem::getRootTypeParameter, return_value_policy<reference_existing_object>())
		.def("setRootSystemParameter", &RootSystem::setRootSystemParameter)
		.def("getRootSystemParameter", &RootSystem::getRootSystemParameter, return_value_policy<reference_existing_object>()) // tutorial "naive (dangerous) approach"
		.def("openFile", &RootSystem::openFile)
		.def("setGeometry", &RootSystem::setGeometry)
		.def("setSoil", &RootSystem::setSoil)
		.def("reset", &RootSystem::reset)
		.def("initialize", &RootSystem::initialize)
		.def("simulate",&RootSystem::simulate)
		.def("getNumberOfNodes", &RootSystem::getNumberOfNodes)
//		.def("getNodes", &RootSystem::getNodes) // TODO something clever to avoid Root, Root*, etc
//		.def("getSegments", &RootSystem::getSegments)
		.def("getRootTips", &RootSystem::getRootTips)
		.def("write",&RootSystem::write)
		.def("setSeed",&RootSystem::setSeed)
	;
	enum_<RootSystem::ScalarTypes>("ScalarType")
	    .value("type", RootSystem::ScalarTypes::st_type)
	    .value("radius", RootSystem::ScalarTypes::st_radius)
	    .value("order", RootSystem::ScalarTypes::st_order)
	    .value("red", RootSystem::ScalarTypes::st_red)
	    .value("green", RootSystem::ScalarTypes::st_green)
	    .value("blue", RootSystem::ScalarTypes::st_blue)
	    .value("time", RootSystem::ScalarTypes::st_time)
	    .value("length", RootSystem::ScalarTypes::st_length)
	    .value("surface", RootSystem::ScalarTypes::st_surface)
	;
	enum_<RootSystem::OutputTypes>("OutputType")
	    .value("segments", RootSystem::OutputTypes::ot_segments)
	    .value("polylines", RootSystem::OutputTypes::ot_polylines)
	;
    /*
     * analysis.h
     */
    class_<AnalysisSDF>("AnalysisSDF",init<RootSystem&>()) //
    	.def(init<AnalysisSDF&>())
		.def("pack", &AnalysisSDF::pack)
		.def("getScalar", &AnalysisSDF::getScalar)
		.def("getSummed", getSummed1)
		.def("getSummed", getSummed2)
        .def("getNumberOfRoots", &AnalysisSDF::getNumberOfRoots)
		.def("write",&AnalysisSDF::write)
    ;

}

/*
 *  currently not exposed..
 *
  	class_<Vector2i>("Vector2i", init<>())
			.def(init<int,int>())
			.def(init<Vector2i&>())
			.def_readwrite("x",&Vector2i::x)
			.def_readwrite("y",&Vector2i::y)
			.def("__str__",&Vector2i::toString)
			;
	class_<Vector2d>("Vector2d", init<>())
			.def(init<double,double>())
			.def(init<Vector2d&>())
			.def_readwrite("x",&Vector2d::x)
			.def_readwrite("y",&Vector2d::y)
			.def("__str__",&Vector2d::toString)
	;
 */

/**
 * solution to wrap vectors from Stackoverflow
 *
// C++ code
typedef std::vector<std::string> MyList;
class MyClass {
  MyList myFuncGet();
  void myFuncSet(const Mylist& list);
  //       stuff
};

// Wrapper code

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;


BOOST_PYTHON_MODULE(mymodule)
{
    class_<MyList>("MyList")
        .def(vector_indexing_suite<MyList>() );

    class_<myClass>("MyClass")
        .def("myFuncGet", &MyClass::myFuncGet)
        .def("myFuncSet", &MyClass::myFuncSet)
        ;
}
*
 */

#endif /* PYTHON_WRAPPER */

#endif /* PY_ROOTBOX_H_ */
