#!/usr/bin/env python

'''
batch_postprocess.py -- a script to submit batched 
jet-image processing jobs.

Author: Luke de Oliveira
'''

import argparse
import sys
import os
import datetime
from subprocess import Popen, PIPE, STDOUT
import logging

LOGGER_PREFIX = ' %s'
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def log(msg):
    logger.info(LOGGER_PREFIX % msg)

####### CHANGE THESE PARAMETERS ###########

BOSON_MASS = 800

PT_HAT_MIN, PT_HAT_MAX = [200, 400]

###########################################

def simulation_dir():
    '''
    Returns the path to all of the simulation code.

    Put in your /path/to/jet-simulations
    '''
    if os.environ['USER'] == 'lukedeo':
        return '/u/at/lukedeo/jet-simulations'
    elif os.environ['USER'] == 'bpn7':
        # -- @bnachman you should change this to your directory.
        return '/nfs/slac/g/atlas/u01/users/bnachman/SLAC_pythia/Reclustering'
    else:
        raise ValueError('Invalid user!')


def generate_script(d):
    '''
    Generates a boilerplate script of the form:

    cd /path/to/jet-simulations
    source ./setup.sh
    ./jetconverter.py --verbose --dump myfile --signal=wprime --chunk=10 file1.root file2.root [...]
    where the flags are filled in by `d`

    `d` should have the following keys:
        * files -- a list of files!
        * dump -- prefix for the outputted root files
        * chunk -- how many input files to put into one output file?
    '''
    return 'cd {}\n'.format(simulation_dir()) + 'source ./setup.sh\n./jetconverter.py --verbose --dump {dump}\
    --chunk {chunk} {files}'.format(**d)

def invoke_bsub(name, queue, log):
    '''
    Starts up bsub, and hangs in a state where a script can be piped in.
    '''
    return 'bsub -J "%s" -o %s -q %s' % (
            name, log, queue
        )


if __name__ == '__main__':

    parser = argparse.ArgumentParser()

    parser.add_argument('--chunk', type=int,
        help='Chunk size (in number of files)', required=True)

    parser.add_argument('--output-dir', type=str, default='./files',
        help='path to directory where the data files should be written')

    parser.add_argument('--file-prefix', type=str, default='PROCESSED',
        help='prefix written to the start of all output files.')

    parser.add_argument('--log-dir', type=str, default='./logs',
        help='Path to directory to write the logs')

    parser.add_argument('files', nargs='*', help='ROOT files to process')

    args = parser.parse_args()

    log('Determining output directories.')

    # -- get the simulation directory
    work_dir = simulation_dir()

    # -- create a timestamped subdir
    subdir = datetime.datetime.now().strftime('%b%d-%H%M%S')

    # -- make the logging directory
    if not os.path.exists(os.path.join(args.log_dir, subdir)):
        os.makedirs(os.path.join(args.log_dir, subdir))

    log('Will write logs to {}'.format(os.path.join(args.log_dir, subdir)))
    
    # -- make the job output directory
    scratch_space = os.path.join(args.output_dir, subdir)
    if not os.path.exists(os.path.join(args.output_dir, subdir)):
        os.makedirs(scratch_space)

    log('Will write samples to {}'.format(scratch_space))

    # log('Script submitted for {} generation.'.format(args.process))
    
    n_files = len(args.files)

    log('Generating ~ {} / {} = {} jobs for processing.'.format(
            n_files, args.chunk, n_files / args.chunk
        )
    )

    chunked_files = [args.files[ix:(ix + args.chunk)] for ix in xrange(0, n_files, args.chunk)]


    # -- which process?


    # for job in xrange(args.jobs):
    for job, FCHUNK in enumerate(chunked_files):
        log('Launching job %s of %s...' % (job + 1, len(chunked_files)))

        # -- dump the bsub output into a nice format
        log_file = os.path.join(args.log_dir, subdir, 'log_job_{}.log'.format(job))

        # -- format the output files with all the relevant kinematic shit
        output_file = '%s_timestamp%s' % (
                args.file_prefix, 
                subdir
            )

        # -- this formats the autogenerated script
        job_params = {
            'dump' : os.path.join(scratch_space, output_file), 
            'files' : FCHUNK, 
            'chunk' : args.chunk
        }

        # -- catalogue where shit is going
        log('Job log is {}.'.format(log_file))
        log('Job output is {}.'.format(os.path.join(scratch_space, output_file)))

        # -- wrap the call to the batch system
        cmd = invoke_bsub('j%sof%s' % (job + 1, args.jobs), 'medium', log_file)

        log('Call is: {}'.format(cmd))

        # -- Open a process...
        job_out = Popen(cmd.split(), stdin=PIPE, stdout=PIPE, stderr=STDOUT)

        # -- ...and feed it the autogen script.
        _ = job_out.communicate(input=generate_script(job_params))
        log('Success.')


