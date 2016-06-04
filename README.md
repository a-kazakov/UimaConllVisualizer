# UIMA-CPP CONLL-U format visualizer

## Foreword

This project is made for [Mathematical Methods of Text Analysis](https://github.com/mathtexts/) course.

It assumed to be run on the following VM: https://yadi.sk/d/Xq-3KjParTntG. However it can be easily adopted to run on any other system. Examine CMakeLists.txt for further information.

## Description

This tool visualizes CONLL-U anotations with support of UIMA-CPP framework. 
It takes CONLL-U file as an input and outputs HTML including source document text and all annotations with corresponding info.

The project includes two UIMA annotators which can also be used separately:
* **CONLL-U format reader** — this annotator reads CONLL-U file to UIMA annotations. Annotations format is specified in ConllReader.xml file.
* **Visualizer** — this annotator takes annotations produced by CONLL-U format reader and produces HTML file with annotations visulized.

## Installation

This guide is written to be used on the following VM: https://yadi.sk/d/Xq-3KjParTntG

Since UIMACPP headers doesn't compile with -std=c++11 option, some changes have to be made on them. Type this command into the terminal to resolve the issue:

`sudo sed -i -e 's/\.operator void...//g' /usr/local/uimacpp/include/uima/strtools.hpp`

Then cd to `/home/student/projects` folder, clone and build the project:

1. `git clone https://github.com/a-kazakov/UimaConllVisualizer.git`
2. `cd UimaConllVisualizer`
3. `cmake .`
4. `make`

The project will be compiled to `/home/student/projects/UimaConllVisualizerOut/`.

## Running

In order to view the result you'll need a web browser, for example, Firefox:

`sudo apt-get install firefox`

After that move to the project destination follder (`/home/student/projects/UimaConllVisualizerOut/`).
`sample.conll` file from the project folder (`/home/student/projects/UimaConllVisualizer`) can be used for the test run. To perform the test run, type the following to console:

`./ConllVisualizer ../UimaConllVisualizer/examples/sample.conll result.html`

When it is done open `result.html` with a web-browser: `firefox result.html`.
