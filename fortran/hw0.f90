program hello
      use, intrinsic :: iso_c_binding
      implicit none

      interface
        function c_getchar() bind(c, name='getchar')
                import :: c_int
                implicit none
                integer(kind=c_int) :: c_getchar
        end function c_getchar
      end interface

      !interface
      !  function c_fadey_init(nx_, ny_, tiles) bind(c, name='fadey_init')
      !          import :: c_int
      !          implicit none
      !          integer(c_int), value :: nx_, ny_, tiles 
      !  end function
      !end interface

      ! declare
      real, allocatable, target :: media(:,:)
      real, allocatable, target :: array(:,:)
      real, allocatable, target :: darray(:,:)
      !real, pointer, dimension(:,:) :: array
      integer :: nx
      integer :: ny
      real :: x
      real :: y
      integer :: k
      integer :: i
      integer :: j
      integer :: inch
      !real, pointer, dimension(:,:) :: arr_ptr      
      real, pointer :: arr_ptr
      real, pointer :: darr_ptr
      real, pointer :: media_ptr
      ! do something
      nx = 111
      ny = 111
      
      print *, 'Hallo!'
      !inch = c_getchar()
      
      allocate(media(nx,ny))
      allocate(array(nx,ny))
      allocate(darray(nx,ny))

      call fadey_init(nx, ny, 3)
      
      arr_ptr => array(1,1)
      darr_ptr => darray(1,1)
      media_ptr => media(1,1)

      do j = 1, ny, 1
        do i = 1, nx, 1
          x = real(i) / real(nx)
          y = real(j) / real(ny)
          !array(i,j) = 1. / (.3+((x-.5)*7.)**2+((y-.5)*13.)**2) + 1. / (.3+((x-.73)*33.)**2+((y-.8)*33.)**2) 
          media(i,j) = .0;
          print *, array(i,j)
        end do
        print *
      end do
 
      do j=ny/2+ny/33, ny/2+ny/7, 1
        do i=nx/2-nx/33, nx-nx/4, 1
          media(i,j) = 7.1
        end do
      end do

      do j = 1, ny, 1
        do i = 1, nx, 1
          x = real(i) / real(nx)
          y = real(j) / real(ny)
          if (media(i,j) > 1) then
            array(i,j) = 1. / (.3+((x-.7)*33.)**2+((y-.67)*33.)**2) 
          end if
          print *, array(i,j)
        end do
        print *
      end do
 


      do j=ny/2-ny/7, ny/2-ny/33, 1
        do i=nx/4 ,nx/2+nx/33, 1
          media(i,j) = 7.1
        end do
      end do

      do j=ny/2-ny/23, ny/2+ny/23, 1
        do i=nx/2-nx/23 ,nx/2+nx/23, 1
          media(i,j) = 13.1
        end do
      end do



      call fadey_draw_real4(arr_ptr, nx, ny, 0)
      call fadey_draw_real4(media_ptr, nx, ny, 1)

      inch = c_getchar()
      
      do k = 1, 3333, 1
        do j = 2, ny-1, 1
          do i = 2, nx-1, 1
           darray(i,j) = - 4. * array(i,j) + array(i-1,j) + array(i+1,j) + array(i,j-1) + array(i,j+1)
          end do
        end do

        do j = 2, ny-1, 1
          do i = 2, nx-1, 1
            array(i,j) = array(i,j) + media(i,j) * darray(i,j) * 0.01
          end do
        end do

        print *, k
        call fadey_draw_real4(arr_ptr, nx, ny, 0)
        call fadey_draw_real4(darr_ptr, nx, ny, 2)
        inch = c_getchar()
      end do

      call fadey_draw_real4(arr_ptr, nx, ny, 0)
      inch = c_getchar()
      
      
      print *, 'bye'
      deallocate(array)
end program hello

