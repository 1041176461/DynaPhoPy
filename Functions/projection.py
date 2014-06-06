import numpy as np


def project_onto_unit_cell(trajectory,q_vector):

    number_of_cell_atoms = trajectory.structure.get_number_of_cell_atoms()
    number_of_dimensions = trajectory.structure.get_number_of_dimensions()
    number_of_atoms = trajectory.structure.number_of_atoms
    atom_type = trajectory.structure.get_atomic_numbers()
    coordinates = trajectory.structure.get_positions()
    velocity = trajectory.get_velocity_mass_average()

    velocity_projected=np.zeros((velocity.shape[0],number_of_cell_atoms,number_of_dimensions),dtype=complex)

    #Projection in primitive cell
    for i in range(number_of_atoms):
        for k in range(number_of_dimensions):
            velocity_projected[:,atom_type[i],k] += velocity[:,i,k]*np.exp(np.complex(0,-1)*np.dot(q_vector,coordinates[i,:]))

    velocity_projected = velocity_projected/(number_of_atoms/number_of_cell_atoms)

    return velocity_projected


def project_onto_phonon(vc,eigenvectors):

    number_of_cell_atoms = vc.shape[1]
    number_of_frequencies = eigenvectors.shape[0]

    #Projection in phonon coordinate
    velocity_projected=np.zeros((vc.shape[0],number_of_frequencies),dtype=complex)

    for k in range (number_of_frequencies):
        for i in range(number_of_cell_atoms):
            velocity_projected[:,k] += np.dot(vc[:,i,:],eigenvectors[k,i,:].conj())
        velocity_projected[:,k] = velocity_projected[:,k]/number_of_cell_atoms

    return velocity_projected


'''

number_of_dimensions = 2
number_of_cell_atoms = 2

eigenvectors = np.array([[0.71067812,0,-0.71067812,0],
                        [-7.542995783e-13,-0.707107,-6.91441e-16,0.707107],
                        [-0.3441510098,0.6177054982,-0.3441510098,0.6177054982],
                        [0.6177054982,0.3441510098,0.6177054982,0.3441510098]])

eigenvectors = np.array([[[eigenvectors [i,j*number_of_dimensions+k] for k in range(number_of_dimensions)] for j in range(number_of_cell_atoms)] for i in range(number_of_cell_atoms*number_of_dimensions)])

q_vector = np.array ([1.5,0.5])


trajectory = read.read_from_file_test()

vc = project_onto_unit_cell(trajectory,q_vector)

#print('3:',trajectory.structure.number_of_atoms)
plt.plot(trajectory.get_time().real,vc[:,0,:].real)
plt.show()

vq = project_onto_phonon(vc,eigenvectors)

plt.plot(trajectory.get_time().real,vq[:,2:4].real)
plt.show()

'''