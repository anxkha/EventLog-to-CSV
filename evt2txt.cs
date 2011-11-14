using System;
using System.Diagnostics.Eventing.Reader;
using System.IO;
using System.Text;

namespace Event_to_Text
{
    class Event_to_Text
    {
        private const int VERSION_MAJOR = 1;
        private const int VERSION_MINOR = 0;

        private const int ERROR_ARGUMENTS       = -1;
        private const int ERROR_FILE_NOT_FOUND  = -2;
        private const int ERROR_BAD_EVENT_FILE  = -3;

        public static int Main(string[] argv)
        {
            EventLogQuery evtQuery;
            EventLogReader evtReader;

            if (argv.Length < 1)
            {
                Console.Error.WriteLine("Event2Text version " + VERSION_MAJOR + "." + VERSION_MINOR);
                Console.Error.WriteLine("USAGE: evt2txt.exe infile");

                return ERROR_ARGUMENTS;
            }

            if (!File.Exists(argv[0]))
            {
                Console.Error.WriteLine("Input file not found: " + argv[0]);

                return ERROR_FILE_NOT_FOUND;
            }

            try
            {
                evtQuery = new EventLogQuery(argv[0], PathType.FilePath);
                evtReader = new EventLogReader(evtQuery);
            }
            catch (Exception e)
            {
                Console.Error.WriteLine("Input file specified is not a valid log file!");
                Console.Error.WriteLine(e);

                return ERROR_BAD_EVENT_FILE;
            }

            for (EventRecord record = evtReader.ReadEvent();
                null != record;
                record = evtReader.ReadEvent())
            {
                Console.WriteLine(record.ToXml());
            }

            return 0;
        }
    }
}
