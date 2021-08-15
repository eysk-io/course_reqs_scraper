# UBC Course Info Scraper

A simple scraper to get UBC course information as provided by http://www.calendar.ubc.ca/vancouver/courses.cfm?page=name and saved to a MongoDB database.

## Getting Started

The program supports building and running on `Linux`. As well, `make` and `gcc` are required. One way to install `make` and `gcc` on your `Linux` machine:

```shell
$ sudo apt-get install make
$ sudo apt-get install gcc
```

In addition, `mongo-c-driver` is required to run the program. For more information and installation: https://github.com/mongodb/mongo-c-driver.

Finally, 

Some projects require initial configuration (e.g. access tokens or keys, `npm i`).
This is the section where you would document those requirements.

## Developing

Here's a brief intro about what a developer must do in order to start developing
the project further:

```shell
git clone https://github.com/your/awesome-project.git
cd awesome-project/
packagemanager install
```

And state what happens step-by-step.

### Building

If your project needs some additional steps for the developer to build the
project after some code changes, state them here:

```shell
./configure
make
make install
```

Here again you should state what actually happens when the code above gets
executed.

## Features

Overall, the program is broken down into three main steps:

1. Get links to each course subject page from http://www.calendar.ubc.ca/vancouver/courses.cfm?page=name.

2. Get each course and its information from each subject page.
    - ex. http://www.calendar.ubc.ca/vancouver/courses.cfm?page=name&code=ADHE for all ADHE course information.

3. Save all course data to a MongoDB database.

#### Upcoming

- Correctly parsed pre/co-requisites and equivalent course information

To speed up the process to retrieve and save course information, the program uses a thread pool - `tpool.c` - to parallelize processes. Specifically, saving each subject's courses is performed by each thread.

## Configuration

Here you should write what are all of the configurations a user can enter when
using the project.

#### Argument 1
Type: `String`  
Default: `'default value'`

State what an argument does and how you can use it. If needed, you can provide
an example below.

Example:
```bash
awesome-project "Some other value"  # Prints "You're nailing this readme!"
```

#### Argument 2
Type: `Number|Boolean`  
Default: 100

Copy-paste as many of these as you need.

## Contributing

If you'd like to contribute, please fork the repository and use a feature branch. Pull requests are welcome.

## Links

- Repository: https://github.com/eyskim/course_reqs_scraper/
- Issue tracker: https://github.com/eyskim/course_reqs_scraper/issues
  - In case of sensitive bugs like security vulnerabilities, please contact `erickim195@gmail.com` directly instead of using issue tracker. Thank you for your efforts to improve the security and privacy of this project!
- Related projects (coming soon):

## Licensing

The code in this project is licensed under MIT license.