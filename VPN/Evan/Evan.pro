######################################################################
# Automatically generated by qmake (2.01a) Thu 10. May 09:35:29 2012
# ... but already edited by husky ;-)
######################################################################

# INCLUDEPATH += . "inc/OpenSceneGraph"  "inc/Qwt" "inc/" "inc/Qwt3D" "inc/DCMTK" "GMM/inc"

THIRD_PARTY_BASE = "../../external"
# EW_BASE = "../../EW"
EW_BASE = "EW" ## removed libew[d] linking and added EW-src files
# OSG_VER = "OSG-2.9.16"
OSG_VER = "OSG-3.0.1"
# OSG_VER = "OSG-3.2.0"
QWT_VER = "qwt-5.2"
QWTPLOT3D_VER = "qwtplot3d-0.2.7"
DCMTK_VER = "dcmtk-3.6.1_20121102"
LAPACK_VER = "lapack-3.4.0"
ICONV_VER = "libiconv-1.14"
XML_VER = "libxml2-2.9.1"

INCLUDEPATH += . 
INCLUDEPATH += "$$THIRD_PARTY_BASE/$$OSG_VER/include" 
INCLUDEPATH += "$$THIRD_PARTY_BASE/$$QWT_VER/include" 
INCLUDEPATH += "$$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include" 
INCLUDEPATH += "$$THIRD_PARTY_BASE/$$DCMTK_VER/include" 
INCLUDEPATH += "$$THIRD_PARTY_BASE/$$LAPACK_VER/include" 
INCLUDEPATH += "$$THIRD_PARTY_BASE/$$ICONV_VER/include" 
INCLUDEPATH += "$$THIRD_PARTY_BASE/$$XML_VER/include/libxml2"
INCLUDEPATH += "$$EW_BASE/include"
INCLUDEPATH += "GMM/include"

# LIBS += -L"lib/OpenSceneGraph" -L"lib/Qwt" -L"lib/ew" -L"lib/Qwt3D" -L"lib/DCMTK" 

LIBS += -L"$$THIRD_PARTY_BASE/$$OSG_VER/lib"
LIBS += -L"$$THIRD_PARTY_BASE/$$QWT_VER/lib" 
LIBS += -L"$$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/lib" 
LIBS += -L"$$THIRD_PARTY_BASE/$$DCMTK_VER/lib" 
LIBS += -L"$$THIRD_PARTY_BASE/$$LAPACK_VER" 
LIBS += -L"$$THIRD_PARTY_BASE/$$XML_VER/lib" 
LIBS += -L"lib/ew"


# QMAKE_LFLAGS = -enable-stdcall-fixup -Wl,-enable-auto-import 
#-Wl,-enable-runtime-pseudo-reloc
 
# unfortunately, this does not work on windows (to be able to move the dlls to a separate directory...)
# QMAKE_LFLAGS = -Wl,-rpath,./:./dlls:./dlls/osgPlugins
 
 
# NOTE: 
# it is important that -lopengl32 is specified AFTER -lew[d]
# otherwise we get lots of linker errors from .o files within libew.a
CONFIG(debug, debug|release) {
    # debug stuff
	## LIBS += -lewd
	
	LIBS += -lqwtd5 -lqwtplot3d_d
	#LIBS += -lqwt5 -lqwtplot3d
	
	QMAKE_CXXFLAGS += -Wall
	# -pedantic
	# -Werror
}
CONFIG(release, debug|release) {
	# release stuff
	## LIBS += -lew
	LIBS += -lqwt5 -lqwtplot3d
}

# LIBS +=  -llapack -lrefblas -ltmglib -lgfortran
LIBS +=  -llapack -lrefblas -lgfortran

LIBS += -llibOpenThreads
LIBS += -llibosg -llibosgManipulator -llibosgViewer -llibosgGA -llibosgUtil -llibosgVolume -llibosgDB

LIBS += -lopengl32 -llibxml2 -lwsock32

QT += xml opengl svg
RC_FILE = Evan.rc


#INCLUDEPATH += "C:/Program Files/R/R-2.11.1/include"
#LIBS +=  -L"C:/Program Files/R/R-2.11.1/bin" -lR
#INCLUDEPATH += "C:/Program Files/R/R-2.15.0/include" "C:/svn/Rcpp/include"
#LIBS +=  -L"C:/Program Files/R/R-2.15.0/bin/i386" -L"C:/svn/Rcpp/lib/i386" -lR -lRcpp
#DEFINES += USE_R_

DEFINES += ENCRYPT_EVAN_LICENSE_
DEFINES += TEMPLAND_TOOLKIT_BUILD=1
DEFINES += QT_USE_NATIVE_WINDOWS

#DEFINES += HAVE_CLASS_TEMPLATE
#DEFINES += HAVE_STATIC_TEMPLATE_METHOD
#DEFINES -= USE_DCMTK

LIBS += -liphlpapi



TEMPLATE = app
TARGET = 
DEPENDPATH += . \
              DataTypes \			  
              Labels \
              MainWindow \
              Nodes \
              release \
              Scheduler \
              Utilities \
              GMM/include \
              GMM/src \
              $$EW_BASE/include/ew \
              $$THIRD_PARTY_BASE/$$QWT_VER/include \
              $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include \
              MainWindow/Designer \
              Nodes/AnnotationNode \
              Nodes/ConcatNode \
              Nodes/ExportNode \
              Nodes/GenerateSurfaceNode \
              Nodes/GPANode \
              Nodes/GroupNode \
              Nodes/ImportNode \
              Nodes/MMRNode \
              Nodes/MultiplyNode \
              Nodes/PCANode \
              Nodes/PCONode \
              Nodes/Plotter3DNode \
              Nodes/PlotterNode \
              Nodes/PLSNode \
              Nodes/PrintNode \
              Nodes/RealiseNode \
              Nodes/ReflectedRelabelling \
              Nodes/RNode \
              Nodes/SelectNode \
              Nodes/SubtractNode \
              Nodes/TemplandNode \
              Nodes/TransformNode \
              Nodes/ViewerNode \
              Nodes/WarperNode \
              $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/config \
              $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata \
              $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle \
              $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd \
			  $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/libi2d \
              Nodes/ExportNode/Designer \
              Nodes/GPANode/Designer \
              Nodes/GroupNode/Designer \
              Nodes/ImportNode/Designer \
              Nodes/MMRNode/Designer \
              Nodes/MultiplyNode/Designer \
              Nodes/PCANode/Designer \
              Nodes/PlotterNode/Designer \
              Nodes/PLSNode/Designer \
              Nodes/PrintNode/Designer \
              Nodes/RNode/Designer \
              Nodes/SelectNode/Designer \
              Nodes/SubtractNode/Designer \
              Nodes/TemplandNode/Designer \
              Nodes/TemplandNode/Viewers \
              Nodes/TemplandNode/ViewTree \
              Nodes/WarperNode/Designer
			  
INCLUDEPATH += .

# Input
HEADERS += DataTypes/CentroidSizes.h \
           DataTypes/DataTypeFactory.h \
           DataTypes/DataTypePtr.h \
           DataTypes/Float.h \
           DataTypes/FloatVector.h \
           DataTypes/IDataType.h \
           DataTypes/IPrintable.h \
           DataTypes/IRenderable.h \
           DataTypes/Labels.h \
           DataTypes/LandmarkSet.h \
           DataTypes/LMKSetRenderable.h \
           DataTypes/Loadings.h \
           DataTypes/MatrixD.h \
           DataTypes/PCScore.h \
           DataTypes/Printables.h \
           DataTypes/ProcrustesDistances.h \
           DataTypes/Renderables.h \
           DataTypes/RList.h \
           DataTypes/SpecimenGroup.h \
           DataTypes/Specimens.h \
           DataTypes/Surface.h \
           DataTypes/SurfaceVector.h \
           DataTypes/TransformMatrix.h \
           DataTypes/Variables.h \
           DataTypes/VectorD.h \
           DataTypes/Volume.h \
           DataTypes/WarpGrid.h \
           DataTypes/Warpings.h \
           Labels/ILabelValue.h \
           Labels/ScalarLabelValue.h \
           Labels/StringLabelValue.h \
           MainWindow/DropArea.h \
           MainWindow/MainWindow.h \
           MainWindow/NodeGraphicsItem.h \
           MainWindow/VPNLayout.h \
           Nodes/INode.h \
           Nodes/INode.hpp \
           Nodes/NodeButton.h \
           Nodes/NodeFactory.h \
           Nodes/NodeLink.h \
           Nodes/NodePort.h \
           Scheduler/Scheduler.h \
           Utilities/ExpressionParser.h \
           Utilities/GeodeFinder.h \
           Utilities/LibraryImport.h \
           Utilities/LicenseKeyUtils.h \
           Utilities/Logger.h \
           Utilities/mexcel.h \
           Utilities/MyRayTracedTechnique.h \
           Utilities/QOSGWidget.h \
           Utilities/rply.h \
           Utilities/sha1.h \
           Utilities/Writer.h \
           Utilities/XMLParser.h \
           Utilities/asa063.hpp \
           GMM/include/eig.h \
           GMM/include/gmm.h \
           GMM/include/gpa.h \
           GMM/include/import.h \
           GMM/include/Matrix.h \
           GMM/include/mmr.h \
           GMM/include/pca.h \
           GMM/include/pco.h \
           GMM/include/pls.h \
           GMM/include/svd.h \
           GMM/include/tpp.h \
           GMM/include/tps.h \
           GMM/include/version.h \
           $$EW_BASE/include/ew/bbox3.h \
           $$EW_BASE/include/ew/curve3.h \
           $$EW_BASE/include/ew/DataflowArray3.h \
           $$EW_BASE/include/ew/DataflowArray3E.h \
           $$EW_BASE/include/ew/DataflowCurve3.h \
           $$EW_BASE/include/ew/DataflowCurve3E.h \
           $$EW_BASE/include/ew/DataflowData3.h \
           $$EW_BASE/include/ew/DataflowForm3.h \
           $$EW_BASE/include/ew/DataflowImage3.h \
           $$EW_BASE/include/ew/DataflowNetwork.h \
           $$EW_BASE/include/ew/DataflowNode.h \
           $$EW_BASE/include/ew/DataflowNotifier.h \
           $$EW_BASE/include/ew/DataflowSpline3.h \
           $$EW_BASE/include/ew/DataflowSurface3.h \
           $$EW_BASE/include/ew/DataflowSurface3E.h \
           $$EW_BASE/include/ew/Debug.h \
           $$EW_BASE/include/ew/DebugHelper.h \
           $$EW_BASE/include/ew/dig3.h \
           $$EW_BASE/include/ew/Dig3SetCurve.h \
           $$EW_BASE/include/ew/Dig3SetSurface.h \
           $$EW_BASE/include/ew/Dig3Space.h \
           $$EW_BASE/include/ew/Dig3Tableau.h \
           $$EW_BASE/include/ew/Dig3TableauSpace.h \
           $$EW_BASE/include/ew/Dig3View.h \
           $$EW_BASE/include/ew/ErrorIO.h \
           $$EW_BASE/include/ew/ErrorLogic.h \
           $$EW_BASE/include/ew/ErrorRuntime.h \
           $$EW_BASE/include/ew/form3.h \
           $$EW_BASE/include/ew/Form3Curve.h \
           $$EW_BASE/include/ew/Form3Embedding.h \
           $$EW_BASE/include/ew/Form3Pointset.h \
           $$EW_BASE/include/ew/Form3Reflection.h \
           $$EW_BASE/include/ew/Form3Surface.h \
           $$EW_BASE/include/ew/Form3Volume.h \
           $$EW_BASE/include/ew/FriendVariable.h \
           $$EW_BASE/include/ew/String.h \
           $$EW_BASE/include/ew/surface3.h \
           $$EW_BASE/include/ew/Time.h \
           $$EW_BASE/include/ew/tps2.h \
           $$EW_BASE/include/ew/tps3.h \
           $$EW_BASE/include/ew/transform2.h \
           $$EW_BASE/include/ew/transform3.h \
           $$EW_BASE/include/ew/version.h \
           $$EW_BASE/include/ew/View3Curve.h \
           $$EW_BASE/include/ew/View3Image.h \
           $$EW_BASE/include/ew/View3Item.h \
           $$EW_BASE/include/ew/View3Landmarks.h \
           $$EW_BASE/include/ew/View3Pick.h \
           $$EW_BASE/include/ew/View3Surface.h \
           $$EW_BASE/include/ew/View3Widget.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_abstract_scale.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_abstract_scale_draw.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_abstract_slider.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_analog_clock.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_array.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_arrow_button.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_clipper.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_color_map.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_compass.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_compass_rose.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_counter.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_curve_fitter.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_data.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_dial.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_dial_needle.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_double_interval.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_double_range.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_double_rect.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_dyngrid_layout.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_event_pattern.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_global.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_interval_data.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_knob.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_layout_metrics.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_legend.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_legend_item.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_legend_itemmanager.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_magnifier.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_math.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_paint_buffer.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_painter.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_panner.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_picker.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_picker_machine.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_canvas.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_curve.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_dict.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_grid.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_item.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_layout.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_magnifier.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_marker.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_panner.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_picker.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_printfilter.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_rasteritem.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_rescaler.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_scaleitem.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_spectrogram.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_plot_zoomer.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_polygon.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_raster_data.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_round_scale_draw.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_scale_div.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_scale_draw.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_scale_engine.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_scale_map.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_scale_widget.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_slider.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_spline.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_symbol.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_text.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_text_engine.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_text_label.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_thermo.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_valuelist.h \
           $$THIRD_PARTY_BASE/$$QWT_VER/include/qwt_wheel.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_autoptr.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_autoscaler.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_axis.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_color.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_colorlegend.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_coordsys.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_drawable.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_enrichment.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_enrichment_std.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_function.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_global.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_graphplot.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_gridmapping.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_helper.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_io.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_io_gl2ps.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_io_reader.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_label.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_mapping.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_multiplot.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_openglhelper.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_parametricsurface.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_plot.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_portability.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_scale.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_surfaceplot.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_types.h \
           $$THIRD_PARTY_BASE/$$QWTPLOT3D_VER/include/qwt3d_volumeplot.h \
           MainWindow/Designer/ui_regInstruct.h \
           Nodes/AnnotationNode/AnnotationNode.h \
           Nodes/ConcatNode/ConcatNode.h \
           Nodes/ExportNode/ExportNode.h \
           Nodes/GenerateSurfaceNode/GenerateSurfaceNode.h \
           Nodes/GPANode/GPANode.h \
           Nodes/GroupNode/GroupNode.h \
           Nodes/GroupNode/GroupTreeWidget.h \
           Nodes/GroupNode/PlotSymbolCombo.h \
           Nodes/ImportNode/ImportFilters.h \
           Nodes/ImportNode/ImportNode.h \
           Nodes/MMRNode/MMRNode.h \
           Nodes/MultiplyNode/MultiplyNode.h \
           Nodes/PCANode/PCANode.h \
           Nodes/PCONode/PCONode.h \
           Nodes/Plotter3DNode/Plotter3DNode.h \
           Nodes/PlotterNode/PlotterNode.h \
           Nodes/PLSNode/PLSNode.h \
           Nodes/PrintNode/PrintNode.h \
           Nodes/RealiseNode/RealiseNode.h \
           Nodes/ReflectedRelabelling/ReflectedRelabelling.h \
           Nodes/RNode/MemBuf.h \
           Nodes/RNode/RInside.h \
           Nodes/RNode/RInsideAutoloads.h \
           Nodes/RNode/RNode.h \
           Nodes/SelectNode/LandmarkSetModel.h \
           Nodes/SelectNode/SelectNode.h \
           Nodes/SelectNode/SpecimensModel.h \
           Nodes/SubtractNode/SubtractNode.h \
           Nodes/TemplandNode/FilmstripPlayerControls.h \
           Nodes/TemplandNode/IdDialog.h \
           Nodes/TemplandNode/MySplitter.h \
           Nodes/TemplandNode/SemiLmkIdDialog.h \
           Nodes/TemplandNode/TableauDialog.h \
           Nodes/TemplandNode/TableauLayout.h \
           Nodes/TemplandNode/TemplandNode.h \
           Nodes/TransformNode/TransformNode.h \
           Nodes/ViewerNode/RenderableTree.h \
           Nodes/ViewerNode/ViewerNode.h \
           Nodes/WarperNode/GridTreeWidget.h \
           Nodes/WarperNode/WarperNode.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/config/osconfig.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/config/osconfig.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/cmdlnarg.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcbytstr.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcchrstr.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dccodec.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcdatset.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcddirif.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcdefine.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcdeftag.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcdicdir.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcdicent.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcdict.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcdirrec.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcelem.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcerror.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcfcache.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcfilefo.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dchashdi.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcistrma.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcistrmb.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcistrmf.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcistrmz.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcitem.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dclist.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcmetinf.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcobject.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcofsetl.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcostrma.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcostrmb.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcostrmf.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcostrmz.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcovlay.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcpath.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcpcache.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcpixel.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcpixseq.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcpxitem.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcrleccd.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcrlecce.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcrlecp.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcrledec.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcrledrg.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcrleenc.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcrleerg.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcrlerp.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcsequen.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcstack.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcswap.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dctag.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dctagkey.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dctk.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dctypes.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcuid.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvr.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrae.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvras.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrat.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrcs.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrda.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrds.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrdt.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrfd.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrfl.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvris.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrlo.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrlt.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrobow.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrof.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrpn.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrpobw.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrsh.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrsl.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrss.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrst.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrtm.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrui.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrul.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrulup.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrus.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcvrut.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcwcache.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/dcxfer.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/vrscan.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dcmimage.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dibaslut.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/diciefn.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dicielut.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dicrvfit.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/didislut.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/didispfn.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/didocu.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/diflipt.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/digsdfn.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/digsdlut.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/diimage.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/diinpx.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/diinpxt.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/diluptab.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dimo1img.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dimo2img.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dimocpt.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dimoflt.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dimoimg.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dimoipxt.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dimomod.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dimoopx.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dimoopxt.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dimopx.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dimopxt.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dimorot.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dimosct.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/diobjcou.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/diovdat.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/diovlay.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/diovlimg.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/diovpln.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dipixel.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/diplugin.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dipxrept.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/diregbas.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/dirotat.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/discalet.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/displint.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/ditranst.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmimgle/diutils.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofalgo.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofbmanip.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofcast.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofcmdln.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofconapp.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofcond.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofconfig.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofconsol.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofcrc32.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofdate.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofdatime.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/offile.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/offname.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofglobal.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/oflist.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/offile.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofoset.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofset.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofsetit.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofstack.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofstd.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofstdinc.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofstream.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofstring.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofthread.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/oftime.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/oftimer.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/oftypes.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/ofstd/ofuoset.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/libi2d/i2d.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/libi2d/i2dbmps.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/libi2d/i2dimgs.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/libi2d/i2djpgs.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/libi2d/i2doutpl.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/libi2d/i2dplnsc.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/libi2d/i2dplsc.h \
           $$THIRD_PARTY_BASE/$$DCMTK_VER/include/dcmtk/dcmdata/libi2d/i2dplvlp.h \
           Nodes/ImportNode/Designer/ui_import.h \
           Nodes/PlotterNode/Designer/ui_axesSettings.h \
           Nodes/SubtractNode/Designer/ui_subtract.h \
           Nodes/TemplandNode/Viewers/View3Qt.h \
           Nodes/TemplandNode/ViewTree/CurveItem.h \
           Nodes/TemplandNode/ViewTree/CurvesTopItem.h \
           Nodes/TemplandNode/ViewTree/FormItem.h \
           Nodes/TemplandNode/ViewTree/GridItem.h \
           Nodes/TemplandNode/ViewTree/LandmarkItem.h \
           Nodes/TemplandNode/ViewTree/LandmarksTopItem.h \
           Nodes/TemplandNode/ViewTree/RefPlanesItem.h \
           Nodes/TemplandNode/ViewTree/SemiLandmarkItem.h \
           Nodes/TemplandNode/ViewTree/SemiLandmarksTopItem.h \
           Nodes/TemplandNode/ViewTree/SliceItem.h \
           Nodes/TemplandNode/ViewTree/SurfaceItem.h \
           Nodes/TemplandNode/ViewTree/SurfaceTopItem.h \
           Nodes/TemplandNode/ViewTree/ViewTree.h \
           Nodes/TemplandNode/ViewTree/ViewTreeItem.h \
           Nodes/TemplandNode/ViewTree/VolumeItem.h \
           Nodes/WarperNode/Designer/ui_createGrid.h \
           Nodes/WarperNode/Designer/ui_gridProperty.h \
           Nodes/WarperNode/Designer/ui_warper.h \
           Utilities/MyReaderWriterDICOM.cpp \
           Utilities/sha1.cpp
FORMS += MainWindow/Designer/license.ui \
         MainWindow/Designer/MainWindow.ui \
         MainWindow/Designer/register.ui \
         MainWindow/Designer/registerInstructions.ui \
         Nodes/ExportNode/Designer/export.ui \
         Nodes/GPANode/Designer/gpa.ui \
         Nodes/GroupNode/Designer/GroupDialog.ui \
         Nodes/ImportNode/Designer/Import.ui \
         Nodes/MMRNode/Designer/mmr.ui \
         Nodes/MultiplyNode/Designer/multiply.ui \
         Nodes/PCANode/Designer/pca.ui \
         Nodes/PCANode/Designer/PCADialog.ui \
         Nodes/PlotterNode/Designer/axes.ui \
         Nodes/PLSNode/Designer/pls.ui \
         Nodes/PrintNode/Designer/print.ui \
         Nodes/RNode/Designer/RDialog.ui \
         Nodes/SelectNode/Designer/SelectDialog.ui \
         Nodes/SubtractNode/Designer/SubtractDialog.ui \
         Nodes/TemplandNode/Designer/IdDialog.ui \
         Nodes/TemplandNode/Designer/MapListDialog.ui \
         Nodes/TemplandNode/Designer/SemiLmkIdDialog.ui \
         Nodes/TemplandNode/Designer/Tab.ui \
         Nodes/TemplandNode/Designer/TableauDialog.ui \
         Nodes/TemplandNode/Designer/TemplandWindow.ui \
         Nodes/WarperNode/Designer/CreateGrid.ui \
         Nodes/WarperNode/Designer/GridProperty.ui \
         Nodes/WarperNode/Designer/Warper.ui
SOURCES += Evan.cpp \
           DataTypes/DataTypeFactory.cpp \
           DataTypes/DataTypePtr.cpp \
           DataTypes/Float.cpp \
           DataTypes/IRenderable.cpp \
           DataTypes/Labels.cpp \
           DataTypes/LandmarkSet.cpp \
           DataTypes/LMKSetRenderable.cpp \
           DataTypes/MatrixD.cpp \
           DataTypes/Printables.cpp \
           DataTypes/RList.cpp \
           DataTypes/SpecimenGroup.cpp \
           DataTypes/Surface.cpp \
           DataTypes/Volume.cpp \
           DataTypes/WarpGrid.cpp \
           MainWindow/DropArea.cpp \
           MainWindow/MainWindow.cpp \
           MainWindow/NodeGraphicsItem.cpp \
           MainWindow/VPNLayout.cpp \
           Nodes/INode.cpp \
           Nodes/NodeFactory.cpp \
           Nodes/NodeLink.cpp \
           Scheduler/Scheduler.cpp \
           Utilities/ExpressionParser.cpp \
           Utilities/Logger.cpp \
           Utilities/mexcel.cpp \
           Utilities/MyRayTracedTechnique.cpp \
           Utilities/MyReaderWriterDICOM.cpp \
           Utilities/QOSGWidget.cpp \
           Utilities/rply.c \
           Utilities/sha1.cpp \
           Utilities/Writer.cpp \
           Utilities/XMLParser.cpp \
           Utilities/asa063.cpp \
		GMM/src/gpa.cpp \
			GMM/src/import.cpp \
			GMM/src/mmr.cpp \
			GMM/src/pca.cpp \
			GMM/src/pco.cpp \
			GMM/src/pls.cpp \
			GMM/src/svd.cpp \
			GMM/src/tpp.cpp \
			GMM/src/tps.cpp \
		$$EW_BASE/src/Bbox3.cpp \
			$$EW_BASE/src/Curve3.cpp \
			$$EW_BASE/src/DataflowArray3.cpp \
			$$EW_BASE/src/DataflowArray3E.cpp \
			$$EW_BASE/src/DataflowCurve3.cpp \
			$$EW_BASE/src/DataflowCurve3E.cpp \
			$$EW_BASE/src/DataflowForm3.cpp \
			$$EW_BASE/src/DataflowImage3.cpp \
			$$EW_BASE/src/DataflowNetwork.cpp \
			$$EW_BASE/src/DataflowNode.cpp \
			$$EW_BASE/src/DataflowNotifier.cpp \
			$$EW_BASE/src/DataflowSpline3.cpp \
			$$EW_BASE/src/DataflowSurface3.cpp \
			$$EW_BASE/src/DataflowSurface3E.cpp \
			$$EW_BASE/src/Debug.cpp \
			$$EW_BASE/src/DebugHelper.cpp \
			$$EW_BASE/src/Dig3.cpp \
			$$EW_BASE/src/Dig3Space.cpp \
			$$EW_BASE/src/Dig3Tableau.cpp \
			$$EW_BASE/src/Dig3View.cpp \
			$$EW_BASE/src/ErrorIO.cpp \
			$$EW_BASE/src/ErrorLogic.cpp \
			$$EW_BASE/src/ErrorRuntime.cpp \
			$$EW_BASE/src/File.cpp \
			$$EW_BASE/src/Form3.cpp \
			$$EW_BASE/src/Gdtoa.cpp \
			$$EW_BASE/src/Geom3.cpp \
			$$EW_BASE/src/Index3.cpp \
			$$EW_BASE/src/String.cpp \
			$$EW_BASE/src/Surface3.cpp \
			$$EW_BASE/src/Time.cpp \
			$$EW_BASE/src/Tps2.cpp \
			$$EW_BASE/src/Tps3.cpp \
			$$EW_BASE/src/Transform2.cpp \
			$$EW_BASE/src/Transform3.cpp \
			$$EW_BASE/src/View3Curve.cpp \
			$$EW_BASE/src/View3Image.cpp \
			$$EW_BASE/src/View3Item.cpp \
			$$EW_BASE/src/View3Landmarks.cpp \
			$$EW_BASE/src/View3Surface.cpp \
			$$EW_BASE/src/View3Widget.cpp \
			$$EW_BASE/src/XmlReader.cpp \
			$$EW_BASE/src/XmlWriter.cpp \
           Nodes/AnnotationNode/AnnotationNode.cpp \
           Nodes/ConcatNode/ConcatNode.cpp \
           Nodes/ExportNode/ExportNode.cpp \
           Nodes/GenerateSurfaceNode/GenerateSurfaceNode.cpp \
           Nodes/GPANode/GPANode.cpp \
           Nodes/GroupNode/GroupNode.cpp \
           Nodes/GroupNode/GroupTreeWidget.cpp \
           Nodes/ImportNode/ImportNode.cpp \
           Nodes/MMRNode/MMRNode.cpp \
           Nodes/MultiplyNode/MultiplyNode.cpp \
           Nodes/PCANode/PCANode.cpp \
           Nodes/PCONode/PCONode.cpp \
           Nodes/Plotter3DNode/Plotter3DNode.cpp \
           Nodes/PlotterNode/PlotterNode.cpp \
           Nodes/PLSNode/PLSNode.cpp \
           Nodes/PrintNode/PrintNode.cpp \
           Nodes/RealiseNode/RealiseNode.cpp \
           Nodes/ReflectedRelabelling/ReflectedRelabelling.cpp \
           Nodes/RNode/MemBuf.cpp \
           Nodes/RNode/RInside.cpp \
           Nodes/RNode/RNode.cpp \
           Nodes/SelectNode/SelectNode.cpp \
           Nodes/SubtractNode/SubtractNode.cpp \
           Nodes/TemplandNode/FilmstripPlayerControls.cpp \
           Nodes/TemplandNode/TableauLayout.cpp \
           Nodes/TemplandNode/TemplandNode.cpp \
           Nodes/TransformNode/TransformNode.cpp \
           Nodes/ViewerNode/RenderableTree.cpp \
           Nodes/ViewerNode/ViewerNode.cpp \
           Nodes/WarperNode/GridTreeWidget.cpp \
           Nodes/WarperNode/WarperNode.cpp \
           Nodes/TemplandNode/Viewers/View3Qt.cpp \
           Nodes/TemplandNode/ViewTree/CurvesTopItem.cpp \
           Nodes/TemplandNode/ViewTree/LandmarksTopItem.cpp \
           Nodes/TemplandNode/ViewTree/SemiLandmarksTopItem.cpp \
           Nodes/TemplandNode/ViewTree/SurfaceTopItem.cpp \
           Nodes/TemplandNode/ViewTree/ViewTree.cpp
RESOURCES += Resources.qrc
