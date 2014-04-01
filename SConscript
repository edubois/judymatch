Import( 'project' )
Import( 'libs' )


judyMatchFlags = { 'LIBPATH': [project.inOutputLib()],
                'CCFLAGS': [project.CC['bigobj'],project.CC['warning3'],project.CC['multithreadedlib'],project.CC['sharedobject']],
                'CXXFLAGS':[project.CC['wchar']],
                'CPPDEFINES':[],
             }

if project.env['mode'] == 'production' :
	judyMatchFlags['CPPDEFINES'].append( 'JUDY_MATCH_PRODUCTION' )
	if 'visibilityhidden' in project.CC:
		judyMatchFlags['SHCCFLAGS'] = [project.CC['visibilityhidden']]

# If your compiler as a flag to mark undefined flags as error in shared libraries
if 'sharedNoUndefined' in project.CC:
	judyMatchFlags['SHLINKFLAGS'] = [project.CC['sharedNoUndefined']]

judyMatch = project.ObjectLibrary( 'judyMatch', envFlags=judyMatchFlags )

project.commonLibs.append( judyMatch )


SConscript( [
              'libraries/judyMatch/SConscript',
            ]
            +
            project.scanFiles( ['applications/example1'], accept=['SConscript'] )
          )

