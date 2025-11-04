clear;clc
% Set folder where the files are
folder = '/Volumes/LaCie/MSc Nuclear Engineering/Thesis/DATAA/0.3um/output_stepping';
outputFolder = fullfile(folder, 'combined_results');  % Create a new folder for combined results
if ~exist(outputFolder, 'dir')
    mkdir(outputFolder);
end

files = dir(fullfile(folder, '*.csv'));

% Prepare a structure to collect files per unique "run"
fileGroups = containers.Map();

% Group files by base name (without _tX.csv)
for k = 1:length(files)
    fname = files(k).name;
    
    % Remove '_tX.csv' at the end
    baseName = regexprep(fname, '_t\d+\.csv$', '');
    
    % Full file path
    fullPath = fullfile(folder, fname);
    
    if isKey(fileGroups, baseName)
        temp = fileGroups(baseName); % Get existing cell array
        temp{end+1} = fullPath;       % Add new file path
        fileGroups(baseName) = temp;  % Reassign it back
    else
        fileGroups(baseName) = {fullPath}; % New cell array
    end
end

% Create a structure to store parameter information
ParamInfo = struct();

% Process each group
keysList = keys(fileGroups);

for i = 1:length(keysList)
    baseName = keysList{i};
    groupFiles = fileGroups(baseName);
    
    % Sort groupFiles by thread number
    threadNumbers = zeros(1, length(groupFiles));
    for j = 1:length(groupFiles)
        [~, name, ~] = fileparts(groupFiles{j});
        tokens = regexp(name, '_t(\d+)$', 'tokens');
        if ~isempty(tokens)
            threadNumbers(j) = str2double(tokens{1}{1});
        else
            error('Could not extract thread number from file: %s', name);
        end
    end
    
    [~, idx] = sort(threadNumbers);
    groupFiles = groupFiles(idx);
    
    % Create a temporary file to store combined data
    combinedFilePath = fullfile(outputFolder, [baseName, '_combined.csv']);
    combinedFileID = fopen(combinedFilePath, 'w');
    
    % Get headers from the first file
    if ~isempty(groupFiles)
        firstFileID = fopen(groupFiles{1}, 'r');
        for j = 1:12
            headerLine = fgetl(firstFileID);
            fprintf(combinedFileID, '%s\n', headerLine);
        end
        fclose(firstFileID);
    end
    
    % Count total rows (escaped electrons)
    totalRows = 0;
    
    % Process each file in the group
    for j = 1:length(groupFiles)
        try
            fileID = fopen(groupFiles{j}, 'r');
            if fileID == -1
                warning('Could not open file: %s', groupFiles{j});
                continue;
            end
            
            % Skip header lines
            for k = 1:12
                fgetl(fileID);
            end
            
            % Read and write all data lines
            dataLine = fgetl(fileID);
            while ischar(dataLine)
                fprintf(combinedFileID, '%s\n', dataLine);
                totalRows = totalRows + 1;
                dataLine = fgetl(fileID);
            end
            
            fclose(fileID);
        catch ME
            warning('Error processing file %s: %s', groupFiles{j}, ME.message);
        end
    end
    
    fclose(combinedFileID);
    
    % Extract parameter information from the base name 
    % Format: 'E4keV_R4.90um_theta20_phi20_nt_data'
    safeFieldName = matlab.lang.makeValidName(baseName);
    
    % Extract energy (keV)
    energyMatch = regexp(baseName, 'E(\d+(?:\.\d+)?)keV', 'tokens');
    if ~isempty(energyMatch)
        energy = energyMatch{1}{1};
        ParamInfo.(safeFieldName).energy = energy;
    else
        ParamInfo.(safeFieldName).energy = 'unknown';
    end
    
    % Extract radius (um)
    radiusMatch = regexp(baseName, 'R(\d+(?:\.\d+)?)um', 'tokens');
    if ~isempty(radiusMatch)
        radius = radiusMatch{1}{1};
        ParamInfo.(safeFieldName).radius = radius;
    else
        ParamInfo.(safeFieldName).radius = 'unknown';
    end
    
    % Extract theta angle
    thetaMatch = regexp(baseName, 'theta(\d+(?:\.\d+)?)', 'tokens');
    if ~isempty(thetaMatch)
        theta = thetaMatch{1}{1};
        ParamInfo.(safeFieldName).theta = theta;
    else
        ParamInfo.(safeFieldName).theta = 'unknown';
    end
    
    % Extract phi angle
    phiMatch = regexp(baseName, 'phi(\d+(?:\.\d+)?)', 'tokens');
    if ~isempty(phiMatch)
        phi = phiMatch{1}{1};
        ParamInfo.(safeFieldName).phi = phi;
    else
        ParamInfo.(safeFieldName).phi = 'unknown';
    end
    
    % Calculate escape probability
    escapeProb = totalRows / 1e5; % assuming 1e5 electrons simulated
    ParamInfo.(safeFieldName).escapeProb = escapeProb;
    ParamInfo.(safeFieldName).numEscaped = totalRows;
end

% Save escape probabilities to a summary file
escapeFile = fullfile(outputFolder, 'escape_probabilities.csv');
fid = fopen(escapeFile, 'w');
fprintf(fid, 'Run,Energy(keV),Radius(um),Theta,Phi,EscapeProbability,NumEscapedElectrons\n');

fieldsList = fieldnames(ParamInfo);
for i = 1:length(fieldsList)
    fieldName = fieldsList{i};
    baseName = regexprep(fieldName, 'x', '');  % Remove any 'x' added by makeValidName
    energy = ParamInfo.(fieldName).energy;
    radius = ParamInfo.(fieldName).radius;
    theta = ParamInfo.(fieldName).theta;
    phi = ParamInfo.(fieldName).phi;
    escapeProb = ParamInfo.(fieldName).escapeProb;
    numEscaped = ParamInfo.(fieldName).numEscaped;
    
    fprintf(fid, '%s,%s,%s,%s,%s,%f,%d\n', baseName, energy, radius, theta, phi, escapeProb, numEscaped);
end

fclose(fid);

disp('Processing complete:');
disp('1. All thread files combined per parameter set');
disp('2. Escape probabilities calculated and saved to "escape_probabilities.csv"');