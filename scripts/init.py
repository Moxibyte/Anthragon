import os
import sys
import json
import zipfile
import tarfile
import subprocess
import urllib.request

# Windows
def GetCodepage():
    return "".join(filter(str.isalnum, subprocess.getoutput('chcp').split(':')[-1].strip()))
    
def FindLatestVisualStudio():
    vswhere = os.getenv('programfiles(x86)') + '\\Microsoft Visual Studio\\Installer\\vswhere.exe'
    out = subprocess.check_output((vswhere, '-latest', '-nocolor', '-format', 'json'))
    return json.loads(out.decode(f'cp{GetCodepage()}'))

def GetVisualStudioYearNumber(vswhere):
    installationVersion = vswhere[0]['installationVersion'].split('.')[0]
    if installationVersion == '17':
        return '2022'
    if installationVersion == '16':
        return '2019'
    if installationVersion == '15':
        return '2017'

def GetVisualStudioPath(vswhere):
    return vswhere[0]['installationPath']

# Premake download
def GetExecutable(exe):
    if sys.platform.startswith('linux'):
        return exe
    else:
        return f'{exe}.exe'

def GetPremakeGenerator():
    if sys.platform.startswith('linux'):
        return 'gmake2'
    else:
        vswhere = FindLatestVisualStudio()
        vsversion = GetVisualStudioYearNumber(vswhere)
        return f'vs{vsversion}'

def GetPremakeDownloadUrl(version):
    baseUrl = f'https://github.com/premake/premake-core/releases/download/v{version}/premake-{version}'
    if sys.platform.startswith('linux'):
        return baseUrl + '-linux.tar.gz'
    else:
        return baseUrl + '-windows.zip'

def DownloadPremake(version = '5.0.0-beta2'):
    premakeDownloadUrl = GetPremakeDownloadUrl(version)
    premakeTargetFolder = './dependencies/premake5'
    premakeTargetZip = f'{premakeTargetFolder}/premake5.tmp'
    premakeTargetExe = f'{premakeTargetFolder}/{GetExecutable("premake5")}'

    if not os.path.exists(premakeTargetExe):
        print('Downloading premake5...')
        os.makedirs(premakeTargetFolder, exist_ok=True)
        urllib.request.urlretrieve(premakeDownloadUrl, premakeTargetZip)

        if premakeDownloadUrl.endswith('zip'):
            with zipfile.ZipFile(premakeTargetZip, 'r') as zipFile:
                zipFile.extract('premake5.exe', premakeTargetFolder)
        else:
            with tarfile.open(premakeTargetZip, 'r') as tarFile:
                tarFile.extract('./premake5', premakeTargetFolder)

# Conan build
def RunConan(configuration):
    subprocess.run((
        'conan', 'install', '.', 
        '--build', 'missing', 
        '--output-folder=./dependencies', 
        '--deployer=full_deploy', 
        f'--settings=build_type={configuration}',
        '--settings=compiler.cppstd=20',
    ))

# Main
if __name__ == '__main__':
    # Download tool applications
    DownloadPremake()

    # Generate conan project
    RunConan('Debug')
    RunConan('Release')

    # Run premake5
    premakeGenerator = GetPremakeGenerator()
    subprocess.run(('./dependencies/premake5/premake5', '--file=./scripts/premake5.lua', premakeGenerator))
