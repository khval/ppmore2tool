#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <workbench/startup.h>

#define sfree(fn,ptr) if (ptr) fn(ptr); ptr = NULL

struct project
{
	const char *filename;
	BPTR dir_lock;
	BPTR old_lock;
};

void free_project(struct project *project)
{
	if (project -> old_lock) SetCurrentDir( project -> old_lock );
	project -> dir_lock = 0;
}

struct DiskObject *dobj = NULL;

bool wbStartup_info(struct WBStartup *wbmsg,struct project *project)
{
	int n;

	printf("%d\n",wbmsg -> sm_NumArgs);

	for (n=0;n<wbmsg -> sm_NumArgs;n++)
	{
		 DebugPrintF( "%d: %s\n",n, wbmsg -> sm_ArgList[n].wa_Name);
	}

 	if ( wbmsg -> sm_NumArgs == 2 )
	{
		project -> filename = wbmsg -> sm_ArgList[1].wa_Name;
		project -> dir_lock = wbmsg -> sm_ArgList[1].wa_Lock;
		project -> old_lock = SetCurrentDir( wbmsg -> sm_ArgList[1].wa_Lock );
	}

	printf("%s\n",project -> filename);

	return project -> filename ? true : false;
}

bool cliStartup(int args,char **arg, struct project *project)
{
	int i,l;

	if (args>1)
	{
		project -> filename = arg[1];
	}

	return project -> filename ? true : false;
}

char *tool = NULL;
const char *multiview = "appdir:multiview";

void run_tool(struct project *project)
{
	BPTR output;
	int buffer_size;
	char *tmp_buffer = NULL;

	buffer_size = strlen( tool ? tool : multiview ) + 1 + 2 + strlen( project -> filename ) + 1;		// space + 2 quote signs + null terminator.

	tmp_buffer = alloca(buffer_size);	// alloc on stack, so we don't need to free it.

	if (tmp_buffer)
	{
		sprintf(tmp_buffer,"%s %c%s%c",
			(tool ? tool : multiview),
			34,
			project -> filename,
			34);

		if ( output = Open("NIL:",MODE_READWRITE))			
		{
			int32 error = SystemTags( tmp_buffer, 
					NP_Input,output, 
					NP_Output,output, 
					NP_CloseInput, FALSE,
					NP_CloseOutput, FALSE,
					TAG_END);

			Close(output);
		}
	}
}

void set_tool(const char *name)
{
	if (name)
	{
		sfree(free,tool);
		tool = strdup(name) ;
	}
}

void set_tool_env()
{
	char tmp[100];
	if (GetVar("ppmore2tool" , tmp, sizeof(tmp), LV_VAR ) != -1) set_tool( tmp );
}

int _main(int args,char **arg)
{
	int i;
	int found;
	struct WBStartup *wb;
	struct project project;

	memset(&project,0,sizeof(project));

	if (args == 0)
	{
		if (wbStartup_info((struct WBStartup *) arg,&project) == false )
		{
			free_project(&project);
			return false;
		}
	}
	else
	{
		if (cliStartup( args, arg, &project ) == false)
		{
			free_project(&project);
			return false;
		}
	}

	set_tool_env();

	dobj = GetDiskObject( project.filename );
	if (dobj)
	{
		set_tool( FindToolType( dobj -> do_ToolTypes, "tool" ) );
		FreeDiskObject(dobj);
	}

	run_tool(&project);

	sfree(free,tool);
	free_project(&project);
}

extern BOOL open_libs();
extern void close_libs();

int main(int argc, char* argv[])
{
	int ret = 122;
	if (open_libs())
	{
		ret = _main( argc, argv );
	}
	close_libs();
	return ret;
}

