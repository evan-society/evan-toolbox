######################################################################
# Automatically generated by qmake (2.01a) Thu 10. May 09:35:29 2012
######################################################################

INCLUDEPATH += . "inc/OpenSceneGraph"  "inc/Qwt" "inc/" "inc/Qwt3D" "inc/DCMTK" "GMM/inc"
#LIBS += -L"lib/OpenSceneGraph" -L"lib/Qwt" -L"lib/ew" -L"lib/Qwt3D" -L"lib/DCMTK" -llibosg -llibosgManipulator -llibosgViewer -llibosgDB -llibosgGA -llibosgUtil -llibosgVolume -lew -lopengl32 -llibxml2 -lwsock32 -llapack_win32
LIBS += -L"lib/OpenSceneGraph" -L"lib/Qwt" -L"lib/ew" -L"lib/Qwt3D" -L"lib/DCMTK" -llibosg -llibosgManipulator -llibosgViewer -llibosgDB -llibosgGA -llibosgUtil -llibosgVolume 

# NOTE: 
# it is important that -lopengl32 is specified AFTER -lew[d]
# otherwise we get lots of linker errors from .o files within libew.a
CONFIG(debug, debug|release) {
    # debug stuff
	LIBS += -lewd
	#LIBS += -lew
	LIBS += -lqwtd5 -lqwtplot3dd
}
CONFIG(release, debug|release) {
	# release stuff
	LIBS += -lew
	LIBS += -lqwt5 -lqwtplot3d
}

LIBS += -lopengl32 -llibxml2 -lwsock32 -llapack_win32

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
              inc \
              Labels \
              MainWindow \
              Nodes \
              release \
              Scheduler \
              Utilities \
              GMM/inc \
              GMM/src \
              inc/EW \
              inc/Qwt \
              inc/Qwt3D \
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
              inc/DCMTK/config \
              inc/DCMTK/dcmdata \
              inc/DCMTK/dcmimgle \
              inc/DCMTK/ofstd \
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
              Nodes/WarperNode/Designer \
              inc/DCMTK/dcmdata/libi2d
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
           GMM/inc/eig.h \
           GMM/inc/gmm.h \
           GMM/inc/gpa.h \
           GMM/inc/import.h \
           GMM/inc/Matrix.h \
           GMM/inc/mmr.h \
           GMM/inc/pca.h \
           GMM/inc/pco.h \
           GMM/inc/pls.h \
           GMM/inc/svd.h \
           GMM/inc/tpp.h \
           GMM/inc/tps.h \
           GMM/inc/version.h \
           inc/EW/bbox3.h \
           inc/EW/curve3.h \
           inc/EW/DataflowArray3.h \
           inc/EW/DataflowArray3E.h \
           inc/EW/DataflowCurve3.h \
           inc/EW/DataflowCurve3E.h \
           inc/EW/DataflowData3.h \
           inc/EW/DataflowForm3.h \
           inc/EW/DataflowImage3.h \
           inc/EW/DataflowNetwork.h \
           inc/EW/DataflowNode.h \
           inc/EW/DataflowNotifier.h \
           inc/EW/DataflowSpline3.h \
           inc/EW/DataflowSurface3.h \
           inc/EW/DataflowSurface3E.h \
           inc/EW/Debug.h \
           inc/EW/DebugHelper.h \
           inc/EW/dig3.h \
           inc/EW/Dig3SetCurve.h \
           inc/EW/Dig3SetSurface.h \
           inc/EW/Dig3Space.h \
           inc/EW/Dig3Tableau.h \
           inc/EW/Dig3TableauSpace.h \
           inc/EW/Dig3View.h \
           inc/EW/ErrorIO.h \
           inc/EW/ErrorLogic.h \
           inc/EW/ErrorRuntime.h \
           inc/EW/form3.h \
           inc/EW/Form3Curve.h \
           inc/EW/Form3Embedding.h \
           inc/EW/Form3Pointset.h \
           inc/EW/Form3Reflection.h \
           inc/EW/Form3Surface.h \
           inc/EW/Form3Volume.h \
           inc/EW/FriendVariable.h \
           inc/EW/String.h \
           inc/EW/surface3.h \
           inc/EW/Time.h \
           inc/EW/tps2.h \
           inc/EW/tps3.h \
           inc/EW/transform2.h \
           inc/EW/transform3.h \
           inc/EW/version.h \
           inc/EW/View3Curve.h \
           inc/EW/View3Image.h \
           inc/EW/View3Item.h \
           inc/EW/View3Landmarks.h \
           inc/EW/View3Pick.h \
           inc/EW/View3Surface.h \
           inc/EW/View3Widget.h \
           inc/Qwt/qwt.h \
           inc/Qwt/qwt_abstract_scale.h \
           inc/Qwt/qwt_abstract_scale_draw.h \
           inc/Qwt/qwt_abstract_slider.h \
           inc/Qwt/qwt_analog_clock.h \
           inc/Qwt/qwt_array.h \
           inc/Qwt/qwt_arrow_button.h \
           inc/Qwt/qwt_clipper.h \
           inc/Qwt/qwt_color_map.h \
           inc/Qwt/qwt_compass.h \
           inc/Qwt/qwt_compass_rose.h \
           inc/Qwt/qwt_counter.h \
           inc/Qwt/qwt_curve_fitter.h \
           inc/Qwt/qwt_data.h \
           inc/Qwt/qwt_dial.h \
           inc/Qwt/qwt_dial_needle.h \
           inc/Qwt/qwt_double_interval.h \
           inc/Qwt/qwt_double_range.h \
           inc/Qwt/qwt_double_rect.h \
           inc/Qwt/qwt_dyngrid_layout.h \
           inc/Qwt/qwt_event_pattern.h \
           inc/Qwt/qwt_global.h \
           inc/Qwt/qwt_interval_data.h \
           inc/Qwt/qwt_knob.h \
           inc/Qwt/qwt_layout_metrics.h \
           inc/Qwt/qwt_legend.h \
           inc/Qwt/qwt_legend_item.h \
           inc/Qwt/qwt_legend_itemmanager.h \
           inc/Qwt/qwt_magnifier.h \
           inc/Qwt/qwt_math.h \
           inc/Qwt/qwt_paint_buffer.h \
           inc/Qwt/qwt_painter.h \
           inc/Qwt/qwt_panner.h \
           inc/Qwt/qwt_picker.h \
           inc/Qwt/qwt_picker_machine.h \
           inc/Qwt/qwt_plot.h \
           inc/Qwt/qwt_plot_canvas.h \
           inc/Qwt/qwt_plot_curve.h \
           inc/Qwt/qwt_plot_dict.h \
           inc/Qwt/qwt_plot_grid.h \
           inc/Qwt/qwt_plot_item.h \
           inc/Qwt/qwt_plot_layout.h \
           inc/Qwt/qwt_plot_magnifier.h \
           inc/Qwt/qwt_plot_marker.h \
           inc/Qwt/qwt_plot_panner.h \
           inc/Qwt/qwt_plot_picker.h \
           inc/Qwt/qwt_plot_printfilter.h \
           inc/Qwt/qwt_plot_rasteritem.h \
           inc/Qwt/qwt_plot_rescaler.h \
           inc/Qwt/qwt_plot_scaleitem.h \
           inc/Qwt/qwt_plot_spectrogram.h \
           inc/Qwt/qwt_plot_zoomer.h \
           inc/Qwt/qwt_polygon.h \
           inc/Qwt/qwt_raster_data.h \
           inc/Qwt/qwt_rect.h \
           inc/Qwt/qwt_round_scale_draw.h \
           inc/Qwt/qwt_scale_div.h \
           inc/Qwt/qwt_scale_draw.h \
           inc/Qwt/qwt_scale_engine.h \
           inc/Qwt/qwt_scale_map.h \
           inc/Qwt/qwt_scale_widget.h \
           inc/Qwt/qwt_slider.h \
           inc/Qwt/qwt_spline.h \
           inc/Qwt/qwt_symbol.h \
           inc/Qwt/qwt_text.h \
           inc/Qwt/qwt_text_engine.h \
           inc/Qwt/qwt_text_label.h \
           inc/Qwt/qwt_thermo.h \
           inc/Qwt/qwt_valuelist.h \
           inc/Qwt/qwt_wheel.h \
           inc/Qwt3D/qwt3d_autoptr.h \
           inc/Qwt3D/qwt3d_autoscaler.h \
           inc/Qwt3D/qwt3d_axis.h \
           inc/Qwt3D/qwt3d_color.h \
           inc/Qwt3D/qwt3d_colorlegend.h \
           inc/Qwt3D/qwt3d_coordsys.h \
           inc/Qwt3D/qwt3d_drawable.h \
           inc/Qwt3D/qwt3d_enrichment.h \
           inc/Qwt3D/qwt3d_enrichment_std.h \
           inc/Qwt3D/qwt3d_function.h \
           inc/Qwt3D/qwt3d_global.h \
           inc/Qwt3D/qwt3d_graphplot.h \
           inc/Qwt3D/qwt3d_gridmapping.h \
           inc/Qwt3D/qwt3d_helper.h \
           inc/Qwt3D/qwt3d_io.h \
           inc/Qwt3D/qwt3d_io_gl2ps.h \
           inc/Qwt3D/qwt3d_io_reader.h \
           inc/Qwt3D/qwt3d_label.h \
           inc/Qwt3D/qwt3d_mapping.h \
           inc/Qwt3D/qwt3d_multiplot.h \
           inc/Qwt3D/qwt3d_openglhelper.h \
           inc/Qwt3D/qwt3d_parametricsurface.h \
           inc/Qwt3D/qwt3d_plot.h \
           inc/Qwt3D/qwt3d_portability.h \
           inc/Qwt3D/qwt3d_scale.h \
           inc/Qwt3D/qwt3d_surfaceplot.h \
           inc/Qwt3D/qwt3d_types.h \
           inc/Qwt3D/qwt3d_volumeplot.h \
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
           inc/DCMTK/config/cfunix.h \
           inc/DCMTK/config/osconfig.h \
           inc/DCMTK/dcmdata/cmdlnarg.h \
           inc/DCMTK/dcmdata/dcbytstr.h \
           inc/DCMTK/dcmdata/dcchrstr.h \
           inc/DCMTK/dcmdata/dccodec.h \
           inc/DCMTK/dcmdata/dcdatset.h \
           inc/DCMTK/dcmdata/dcddirif.h \
           inc/DCMTK/dcmdata/dcdebug.h \
           inc/DCMTK/dcmdata/dcdefine.h \
           inc/DCMTK/dcmdata/dcdeftag.h \
           inc/DCMTK/dcmdata/dcdicdir.h \
           inc/DCMTK/dcmdata/dcdicent.h \
           inc/DCMTK/dcmdata/dcdict.h \
           inc/DCMTK/dcmdata/dcdirrec.h \
           inc/DCMTK/dcmdata/dcelem.h \
           inc/DCMTK/dcmdata/dcerror.h \
           inc/DCMTK/dcmdata/dcfcache.h \
           inc/DCMTK/dcmdata/dcfilefo.h \
           inc/DCMTK/dcmdata/dchashdi.h \
           inc/DCMTK/dcmdata/dcistrma.h \
           inc/DCMTK/dcmdata/dcistrmb.h \
           inc/DCMTK/dcmdata/dcistrmf.h \
           inc/DCMTK/dcmdata/dcistrmz.h \
           inc/DCMTK/dcmdata/dcitem.h \
           inc/DCMTK/dcmdata/dclist.h \
           inc/DCMTK/dcmdata/dcmetinf.h \
           inc/DCMTK/dcmdata/dcobject.h \
           inc/DCMTK/dcmdata/dcofsetl.h \
           inc/DCMTK/dcmdata/dcostrma.h \
           inc/DCMTK/dcmdata/dcostrmb.h \
           inc/DCMTK/dcmdata/dcostrmf.h \
           inc/DCMTK/dcmdata/dcostrmz.h \
           inc/DCMTK/dcmdata/dcovlay.h \
           inc/DCMTK/dcmdata/dcpath.h \
           inc/DCMTK/dcmdata/dcpcache.h \
           inc/DCMTK/dcmdata/dcpixel.h \
           inc/DCMTK/dcmdata/dcpixseq.h \
           inc/DCMTK/dcmdata/dcpxitem.h \
           inc/DCMTK/dcmdata/dcrleccd.h \
           inc/DCMTK/dcmdata/dcrlecce.h \
           inc/DCMTK/dcmdata/dcrlecp.h \
           inc/DCMTK/dcmdata/dcrledec.h \
           inc/DCMTK/dcmdata/dcrledrg.h \
           inc/DCMTK/dcmdata/dcrleenc.h \
           inc/DCMTK/dcmdata/dcrleerg.h \
           inc/DCMTK/dcmdata/dcrlerp.h \
           inc/DCMTK/dcmdata/dcsequen.h \
           inc/DCMTK/dcmdata/dcstack.h \
           inc/DCMTK/dcmdata/dcswap.h \
           inc/DCMTK/dcmdata/dctag.h \
           inc/DCMTK/dcmdata/dctagkey.h \
           inc/DCMTK/dcmdata/dctk.h \
           inc/DCMTK/dcmdata/dctypes.h \
           inc/DCMTK/dcmdata/dcuid.h \
           inc/DCMTK/dcmdata/dcvm.h \
           inc/DCMTK/dcmdata/dcvr.h \
           inc/DCMTK/dcmdata/dcvrae.h \
           inc/DCMTK/dcmdata/dcvras.h \
           inc/DCMTK/dcmdata/dcvrat.h \
           inc/DCMTK/dcmdata/dcvrcs.h \
           inc/DCMTK/dcmdata/dcvrda.h \
           inc/DCMTK/dcmdata/dcvrds.h \
           inc/DCMTK/dcmdata/dcvrdt.h \
           inc/DCMTK/dcmdata/dcvrfd.h \
           inc/DCMTK/dcmdata/dcvrfl.h \
           inc/DCMTK/dcmdata/dcvris.h \
           inc/DCMTK/dcmdata/dcvrlo.h \
           inc/DCMTK/dcmdata/dcvrlt.h \
           inc/DCMTK/dcmdata/dcvrobow.h \
           inc/DCMTK/dcmdata/dcvrof.h \
           inc/DCMTK/dcmdata/dcvrpn.h \
           inc/DCMTK/dcmdata/dcvrpobw.h \
           inc/DCMTK/dcmdata/dcvrsh.h \
           inc/DCMTK/dcmdata/dcvrsl.h \
           inc/DCMTK/dcmdata/dcvrss.h \
           inc/DCMTK/dcmdata/dcvrst.h \
           inc/DCMTK/dcmdata/dcvrtm.h \
           inc/DCMTK/dcmdata/dcvrui.h \
           inc/DCMTK/dcmdata/dcvrul.h \
           inc/DCMTK/dcmdata/dcvrulup.h \
           inc/DCMTK/dcmdata/dcvrus.h \
           inc/DCMTK/dcmdata/dcvrut.h \
           inc/DCMTK/dcmdata/dcwcache.h \
           inc/DCMTK/dcmdata/dcxfer.h \
           inc/DCMTK/dcmdata/vrscan.h \
           inc/DCMTK/dcmimgle/dcmimage.h \
           inc/DCMTK/dcmimgle/dibaslut.h \
           inc/DCMTK/dcmimgle/diciefn.h \
           inc/DCMTK/dcmimgle/dicielut.h \
           inc/DCMTK/dcmimgle/dicrvfit.h \
           inc/DCMTK/dcmimgle/didislut.h \
           inc/DCMTK/dcmimgle/didispfn.h \
           inc/DCMTK/dcmimgle/didocu.h \
           inc/DCMTK/dcmimgle/diflipt.h \
           inc/DCMTK/dcmimgle/digsdfn.h \
           inc/DCMTK/dcmimgle/digsdlut.h \
           inc/DCMTK/dcmimgle/diimage.h \
           inc/DCMTK/dcmimgle/diinpx.h \
           inc/DCMTK/dcmimgle/diinpxt.h \
           inc/DCMTK/dcmimgle/diluptab.h \
           inc/DCMTK/dcmimgle/dimo1img.h \
           inc/DCMTK/dcmimgle/dimo2img.h \
           inc/DCMTK/dcmimgle/dimocpt.h \
           inc/DCMTK/dcmimgle/dimoflt.h \
           inc/DCMTK/dcmimgle/dimoimg.h \
           inc/DCMTK/dcmimgle/dimoipxt.h \
           inc/DCMTK/dcmimgle/dimomod.h \
           inc/DCMTK/dcmimgle/dimoopx.h \
           inc/DCMTK/dcmimgle/dimoopxt.h \
           inc/DCMTK/dcmimgle/dimopx.h \
           inc/DCMTK/dcmimgle/dimopxt.h \
           inc/DCMTK/dcmimgle/dimorot.h \
           inc/DCMTK/dcmimgle/dimosct.h \
           inc/DCMTK/dcmimgle/diobjcou.h \
           inc/DCMTK/dcmimgle/diovdat.h \
           inc/DCMTK/dcmimgle/diovlay.h \
           inc/DCMTK/dcmimgle/diovlimg.h \
           inc/DCMTK/dcmimgle/diovpln.h \
           inc/DCMTK/dcmimgle/dipixel.h \
           inc/DCMTK/dcmimgle/diplugin.h \
           inc/DCMTK/dcmimgle/dipxrept.h \
           inc/DCMTK/dcmimgle/diregbas.h \
           inc/DCMTK/dcmimgle/dirotat.h \
           inc/DCMTK/dcmimgle/discalet.h \
           inc/DCMTK/dcmimgle/displint.h \
           inc/DCMTK/dcmimgle/ditranst.h \
           inc/DCMTK/dcmimgle/diutils.h \
           inc/DCMTK/ofstd/ofalgo.h \
           inc/DCMTK/ofstd/ofbmanip.h \
           inc/DCMTK/ofstd/ofcast.h \
           inc/DCMTK/ofstd/ofcmdln.h \
           inc/DCMTK/ofstd/ofconapp.h \
           inc/DCMTK/ofstd/ofcond.h \
           inc/DCMTK/ofstd/ofconfig.h \
           inc/DCMTK/ofstd/ofconsol.h \
           inc/DCMTK/ofstd/ofcrc32.h \
           inc/DCMTK/ofstd/ofdate.h \
           inc/DCMTK/ofstd/ofdatime.h \
           inc/DCMTK/ofstd/offile.h \
           inc/DCMTK/ofstd/offname.h \
           inc/DCMTK/ofstd/ofglobal.h \
           inc/DCMTK/ofstd/oflist.h \
           inc/DCMTK/ofstd/oflogfil.h \
           inc/DCMTK/ofstd/ofoset.h \
           inc/DCMTK/ofstd/ofset.h \
           inc/DCMTK/ofstd/ofsetit.h \
           inc/DCMTK/ofstd/ofstack.h \
           inc/DCMTK/ofstd/ofstd.h \
           inc/DCMTK/ofstd/ofstdinc.h \
           inc/DCMTK/ofstd/ofstream.h \
           inc/DCMTK/ofstd/ofstring.h \
           inc/DCMTK/ofstd/ofthread.h \
           inc/DCMTK/ofstd/oftime.h \
           inc/DCMTK/ofstd/oftimer.h \
           inc/DCMTK/ofstd/oftypes.h \
           inc/DCMTK/ofstd/ofuoset.h \
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
           inc/DCMTK/dcmdata/libi2d/i2d.h \
           inc/DCMTK/dcmdata/libi2d/i2dbmps.h \
           inc/DCMTK/dcmdata/libi2d/i2dimgs.h \
           inc/DCMTK/dcmdata/libi2d/i2djpgs.h \
           inc/DCMTK/dcmdata/libi2d/i2doutpl.h \
           inc/DCMTK/dcmdata/libi2d/i2dplnsc.h \
           inc/DCMTK/dcmdata/libi2d/i2dplsc.h \
           inc/DCMTK/dcmdata/libi2d/i2dplvlp.h \
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
