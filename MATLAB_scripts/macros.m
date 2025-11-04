clear;clc
% Define parameters
%energies_keV = 11;
energies_keV = [17 15 13 11 9 7 6 5 4 3 1 0.5 0.2];
%radii_um = [0.14 0.13 0.12 0.11 0.1 0.09 0.08 0.07 0.05 0.03 0.01 0.0];
radii_um = 0.15;
%angletheta = 0:20:180; 
angletheta = [100 120 140 160 180];
anglephi = 0;
%angles = [90 135 180];
N_particles = 1e5;
particle = 'e-';

% Create output directory
outDir = 'macros150_surface';
if ~exist(outDir, 'dir')
    mkdir(outDir);
end

% Loop to create macro files
for e = energies_keV
    for r = radii_um
        for phi = anglephi
            for theta = angletheta

                % Convert to radians and spherical coordinates for direction
                theta_rad = deg2rad(theta);
                phi_rad = deg2rad(phi);
                dx = sin(theta_rad) * cos(phi_rad);
                dy = sin(theta_rad) * sin(phi_rad);
                dz = cos(theta_rad);

                % Define file name
                filename = sprintf('E%dkeV_R%.2fum_theta%d_phi%d.mac', e, r, theta, phi);
                filepath = fullfile(outDir, filename);

                % Open file to write
                fid = fopen(filepath, 'w');

                % Write macro content
                fprintf(fid, '/gps/particle %s\n', particle);
                fprintf(fid, '/gps/pos/centre %.2f %.2f %.2f micrometer\n', 0, 0, r);
                fprintf(fid, '/gps/direction %.6f %.6f %.6f\n', dx, dy, dz); 
                fprintf(fid, '/gps/ene/mono %d eV\n', e*1000);
                fprintf(fid, '/run/setRunName E%dkeV_R%.2fum_theta%d_phi%d\n', e, r, theta, phi);
                fprintf(fid, '/run/beamOn %d\n', N_particles);

                % Close file
                fclose(fid);
            end
        end
    end
end

disp('Macro file generation complete.');


% Save list of all macro filenames for batch execution
fileList = fopen(fullfile(outDir, 'macro_list.txt'), 'w');
for e = energies_keV
    for r = radii_um
        for phi = anglephi
            for theta = angletheta
                filename = sprintf('E%dkeV_R%.2fum_theta%d_phi%d.mac', e, r, theta, phi);
                % build the path and then swap “\” → “/”
                rawpath = fullfile(outDir, filename);
                unixpath = strrep(rawpath, '\', '/');
                fprintf(fileList, '%s\n', unixpath);
            end
        end
    end
end
fclose(fileList);

disp('Macro list file created: macro_list.txt');
