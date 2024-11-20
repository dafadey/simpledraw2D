program hello
      use, intrinsic :: iso_c_binding
      implicit none

      ! declare
      real, allocatable, target :: media(:,:)
      real, allocatable, target :: array(:,:)
      real, allocatable, target :: darray(:,:)
      !real, pointer, dimension(:,:) :: array
      integer :: nx
      integer :: ny
      real :: x
      real :: y
      real :: qxp
      real :: qxn
      real :: qyp
      real :: qyn
      integer :: k
      integer :: i
      integer :: j
      !real, pointer, dimension(:,:) :: arr_ptr      
      real, pointer :: arr_ptr
      real, pointer :: darr_ptr
      real, pointer :: media_ptr
      ! do something
      nx = 333
      ny = 333
      
      print *, 'Hallo!'
      
      allocate(media(nx,ny))
      allocate(array(nx,ny))
      allocate(darray(nx,ny))

      call fadey_init(nx, ny, 3)
      
      arr_ptr => array(0,0)
      darr_ptr => darray(0,0)
      media_ptr => media(0,0)

      do j = 0, ny-1, 1
        do i = 0, nx-1, 1
          x = real(i) / real(nx)
          y = real(j) / real(ny)
          media(i,j) = .0;
          darray(i,j) = .0;
          print *, array(i,j)
        end do
        print *
      end do
 
      do j=ny/2+ny/33, ny/2+ny/7, 1
        do i=nx/2-nx/33, nx-nx/4, 1
          media(i,j) = 3.1
        end do
      end do

      do j = 0, ny-1, 1
        do i = 0, nx-1, 1
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
          media(i,j) = 3.1
        end do
      end do

      do j=ny/2-ny/23, ny/2+ny/23, 1
        do i=nx/2-nx/23 ,nx/2+nx/23, 1
          media(i,j) = 13.1
        end do
      end do

      call fadey_draw_real4(arr_ptr, nx, ny, 0)
      call fadey_draw_real4(media_ptr, nx, ny, 1)

      call fadey_halt()

      do k = 1, 3333, 1
        do j = 1, ny-2, 1
          do i = 1, nx-2, 1
           qxp = array(i+1,j) - array(i,j)
           qxn = array(i-1,j) - array(i,j)
           qyp = array(i,j+1) - array(i,j)
           qyn = array(i,j-1) - array(i,j)
           
           darray(i,j) = qxp * media(i+1,j) + qxn * media(i-1,j) + qyp  * media(i,j+1) + qyn * media(i,j-1)
          end do
        end do

        do j = 1, ny-2, 1
          do i = 1, nx-2, 1
            array(i,j) = array(i,j) + darray(i,j) * 0.01
          end do
        end do

        print *, k
        call fadey_draw_real4(arr_ptr, nx, ny, 0)
        call fadey_draw_real4(darr_ptr, nx, ny, 2)
      end do

      call fadey_draw_real4(arr_ptr, nx, ny, 0)
      
      
      print *, 'bye'
      deallocate(array)
end program hello

