from setuptools import find_packages, setup
import os
from glob import glob
package_name = 'my_py_pkg'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
    ("share/ament_index/resource_index/packages",
     ["resource/" + package_name]),
    (os.path.join("share", package_name), ["package.xml"]),
    (os.path.join("share", package_name, "launch"),
     glob("launch/*.py") + glob("launch/*.xml")),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='lunog',
    maintainer_email='vul45845@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            "my_first_node = my_py_pkg.my_first_node:main",
            "robot_news_station = my_py_pkg.robot_news_station:main",
            "smartphone = my_py_pkg.smartphone:main",
            "service_server = my_py_pkg.service_server:main",
            "service_client = my_py_pkg.service_client:main",
            'static_frame_publisher = my_py_pkg.static_frame_publisher:main',
            'dynamic_frame_publisher = my_py_pkg.static_frame_publisher:main',
            'tf_listener = my_py_pkg.tf_listener:main'
        ],
    }, 
)
