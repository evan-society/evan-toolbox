@ECHO OFF
CMD /C make clean
CMD /C make
CMD /C copy ..\lib\libewd.a ..\..\VPN\Evan\lib\EW\
@ECHO ON