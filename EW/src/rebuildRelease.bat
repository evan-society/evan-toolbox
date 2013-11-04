@ECHO OFF
CMD /C make clean
CMD /C make NODEBUG='1'
CMD /C copy ..\lib\libew.a ..\..\VPN\Evan\lib\EW\
@ECHO ON